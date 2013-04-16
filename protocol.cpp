#include <stdlib.h>
#include <boost/lexical_cast.hpp>
#include "mavlink/ardrone/mavlink.h"
#include "protocol.h"
using namespace std;

arproxy::ProtocolHandler::ProtocolHandler(void) : sequence_number(0), navdata_sequence_number(0) {}

/**
 * TODO: check that maximum message size is not exceeded before appending sub-message
 */
 
void arproxy::ProtocolHandler::serial2network(const std::vector<char>& serial_packet, size_t serial_bytes,
  std::vector<char>& network_packet, size_t& network_bytes)
{
  mavlink_message_t msg;
	mavlink_status_t status;
  
  //cout << "parsing serial messages in " << serial_bytes << " bytes (" << serial_packet.size() << ")" << endl;
  
  std::string network_str; 
  for (uint i = 0; i < serial_bytes; i++) {
    if (mavlink_parse_char(MAVLINK_COMM_0, serial_packet[i], &msg, &status)) {
      switch (msg.msgid) {
        case MAVLINK_MSG_ID_HEARTBEAT:
          cout << "received heartbeat" << endl;
          network_str += prepare_network_msg("AT*COMWDG");
        break;
        case MAVLINK_MSG_ID_SET_LEDS:
        {
          cout << "received set leds command" << endl;
          mavlink_set_leds_t set_leds_msg;
          mavlink_msg_set_leds_decode(&msg, &set_leds_msg);
          std::string animation_str = boost::lexical_cast<std::string>(set_leds_msg.animation);
          std::string frequency_str = boost::lexical_cast<std::string>(*(int32_t*)(&set_leds_msg.frequency));
          std::string duration_str = boost::lexical_cast<std::string>(set_leds_msg.duration);
          //network_str += prepare_network_msg("AT*LED", animation_str + "," + frequency_str + "," + duration_str);
          network_str += prepare_network_msg("AT*LED", "9,1065353216,5");
        }
        break;
        case MAVLINK_MSG_ID_TAKEOFF_LAND:
        {
          cout << "received takeoff land" << endl;
          mavlink_takeoff_land_t takeoff_land_msg;
          mavlink_msg_takeoff_land_decode(&msg, &takeoff_land_msg);
          uint32_t bits = 0;
          bits |= (1 << 18) | (1 << 20) | (1 << 22) | (1 << 24) | (1 << 28); // see SDK developer guide
          if (takeoff_land_msg.action == 0) {
            bits |= (1 << 9); // takeoff bit on
            network_str += prepare_network_msg("AT*FTRIM"); // set trim before takeoff
          }
          else {
            bits &= ~(1 << 9); // takeoff bit off
          }
          network_str += prepare_network_msg("AT*REF", boost::lexical_cast<std::string>(bits));
        } 
        break;
        case MAVLINK_MSG_ID_SET_PCMD:
        {
          //cout << "received pcmd" << endl;
          mavlink_set_pcmd_t set_pcmd_msg;
          mavlink_msg_set_pcmd_decode(&msg, &set_pcmd_msg);
          std::string roll_str = boost::lexical_cast<std::string>(*(uint32_t*)(&set_pcmd_msg.roll));
          std::string pitch_str = boost::lexical_cast<std::string>(*(uint32_t*)(&set_pcmd_msg.pitch));
          std::string vspeed_str = boost::lexical_cast<std::string>(*(uint32_t*)(&set_pcmd_msg.vspeed));
          std::string yaw_str = boost::lexical_cast<std::string>(*(uint32_t*)(&set_pcmd_msg.yaw));
          uint32_t flags = set_pcmd_msg.hover;
          std::string flag_str = boost::lexical_cast<std::string>(flags);
          network_str += prepare_network_msg("AT*PCMD", flag_str + "," + roll_str + "," + pitch_str + "," + vspeed_str + "," + yaw_str);          
        }
        break;
        case MAVLINK_MSG_ID_ENABLE_NAVDATA:
        {
          cout << "received enable navdata" << endl;
          mavlink_enable_navdata_t enable_navdata;
          mavlink_msg_enable_navdata_decode(&msg, &enable_navdata);
          network_str += prepare_network_msg("AT*CONFIG",std::string("\"general:navdata_demo\",") + (enable_navdata.enable ? "\"TRUE\"" : "\"FALSE\""));
          network_str += prepare_network_msg("AT*CTRL=") + boost::lexical_cast<std::string>((uint)ACK_CONTROL_MODE) + ",0\r";
        }
        break;
        default:
          cout << "received unhandled message" << endl;
        break;
      }
    }
  }
  //cout << "network_str: " << network_str << endl;
  
  if (network_str.size() > network_packet.size()) cout << "exceeded buffer size!" << endl;
  if (!network_str.empty()) std::copy(network_str.begin(), network_str.begin() + network_str.size(), network_packet.begin());
  network_bytes = network_str.size();
  //cout << "commiting network packet of " << network_bytes << " bytes" << endl;
}

void arproxy::ProtocolHandler::network2serial(const std::vector<char>& network_packet, size_t network_bytes, 
  std::vector<char>& serial_packet, size_t& serial_bytes)
{
  serial_bytes = 0;
  
  navdata_t* navdata = (navdata_t*)&network_packet[0];
  if (navdata->header == NAVDATA_HEADER) {
    //cout << "received navdata packet of " << network_bytes << " bytes" << endl;
    if (navdata->mykonos_state & MYKONOS_COM_WATCHDOG_MASK) {
      //cout << "resetting seq number" << endl;
      navdata_sequence_number = 0; 
    }
    
    /*if (navdata->mykonos_state & MYKONOS_NAVDATA_BOOTSTRAP) cout << "boostrap mode" << endl;
    if (navdata->mykonos_state & MYKONOS_NAVDATA_DEMO_MASK) cout << "demo mode" << endl;
    if (navdata->mykonos_state & MYKONOS_COMMAND_MASK) cout << "command mask" << endl;*/
    
    if ((navdata->mykonos_state & MYKONOS_NAVDATA_DEMO_MASK) && navdata->sequence > navdata_sequence_number) {
      
      navdata_demo_t demo_data;
      navdata_cks_t cks_data; 
      if (!navdata_unpack(navdata, &demo_data, &cks_data)) return;
      
      // compute checksum
      uint32_t cks = 0;
      for(uint i = 0; i < network_bytes - sizeof(navdata_cks_t); i++ )
        cks += (uint32_t)network_packet[i];      
      if (cks != cks_data.cks) { cout << "wrong checksum (" << cks << "!=" << cks_data.cks << "), dropping packet" << endl; return; }
      //else { cout << "cks match " << cks << " " << cks_data.cks << endl; }
      
      /*cout << "bat:" << (double)demo_data.vbat_flying_percentage << " alt " << demo_data.altitude << 
        " angles: " << demo_data.theta << " " << demo_data.phi << " " << demo_data.psi <<
        " speeds: " << demo_data.vx << " " << demo_data.vy << " " << demo_data.vz << endl;*/
      mavlink_message_t msg;
      mavlink_msg_navdata_pack(0, 0, &msg, /*timestamp,*/ demo_data.vbat_flying_percentage, demo_data.phi, demo_data.psi, demo_data.theta,
        demo_data.altitude, demo_data.vx, demo_data.vy, demo_data.vz);
      serial_bytes = mavlink_msg_to_send_buffer((uint8_t*)&serial_packet[0], &msg);  
    }
    navdata_sequence_number = navdata->sequence;
  }
}

bool arproxy::ProtocolHandler::navdata_unpack(const navdata_t* navdata, navdata_demo_t* demo_data, navdata_cks_t* cks_data)
{
  bool got_demo_data = false;
  navdata_option_t* navdata_option_ptr = (navdata_option_t*)&navdata->options[0];
  while (true) {
    //cout << "tag: " << navdata_option_ptr->tag << endl;    
    
    if (navdata_option_ptr->size == 0) { cout << "option of zero size" << endl; break; }
    //cout << "unpacking " << navdata_option_ptr->size << " bytes" << endl;
    
    if (navdata_option_ptr->tag == NAVDATA_DEMO_TAG) {
      //cout << "unpacking demo (" << sizeof(navdata_demo_t) << ")" << endl;
      if (navdata_option_ptr->size != sizeof(navdata_demo_t)) cout << "incorrect data size " << navdata_option_ptr->size << " instead of " << sizeof(navdata_demo_t) << " (wrong API?)" << endl;
      else {
        memcpy(demo_data, navdata_option_ptr, navdata_option_ptr->size);
        got_demo_data = true;
      }
    }
    else if (navdata_option_ptr->tag == NAVDATA_CKS_TAG) {
      //cout << "unpacking cks" << endl;
      if (navdata_option_ptr->size != sizeof(navdata_cks_t)) cout << "incorrect data size " << navdata_option_ptr->size << " instead of " << sizeof(navdata_cks_t) << " (wrong API?)" << endl;
      else memcpy(cks_data, navdata_option_ptr, navdata_option_ptr->size);
      //cout << "cks: " << cks_data->cks << endl;
      break;
    }
    //else { cout << "unhandled navdata option" << endl; }
    
    navdata_option_ptr = (navdata_option_t*)((uint8_t*)navdata_option_ptr + navdata_option_ptr->size);
  }
  return got_demo_data;
}

std::string arproxy::ProtocolHandler::prepare_network_msg(const std::string& msg, const std::string& parameters)
{
  sequence_number++;
  std::string output = msg + "=" + boost::lexical_cast<std::string>(sequence_number) + (parameters.empty() ? "" : ",") + parameters;
  //cout << "msg: " << output << endl;
  return output + "\r";
}

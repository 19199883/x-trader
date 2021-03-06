﻿#include <iostream>     // std::cin, std::cout
#include <fstream>
#include "quote_cmn_utility.h"
#include "vrt_value_obj.h"

using namespace std;

IPAndPortNum ParseIPAndPortNum(const std::string &addr_cfg)
{
    //format: udp://192.168.60.23:7120   or  tcp://192.168.60.23:7120
    std::string ip_port = addr_cfg.substr(6);
    std::size_t split_pos = ip_port.find(":");
    if ((split_pos == std::string::npos) || (split_pos + 1 >= ip_port.length())){
        clog_error("parse address failed: %s", addr_cfg.c_str());
        return std::make_pair("", (unsigned short) 0);
    }

    std::string addr_ip = ip_port.substr(0, split_pos);
    std::string addr_port = ip_port.substr(split_pos + 1);
    int port_tmp = atoi(addr_port.c_str());
    if (port_tmp <= 0 || port_tmp > 0xFFFF)
    {
        clog_error("port in address beyond valid range: %s", addr_cfg.c_str());
        return std::make_pair("", 0);
    }

    return std::make_pair(addr_ip, (unsigned short) port_tmp);
}

IPAndPortStr ParseIPAndPortStr(const std::string &addr_cfg)
{
    //format: udp://192.168.60.23:7120   or  tcp://192.168.60.23:7120
    std::string ip_port = addr_cfg.substr(6);
    std::size_t split_pos = ip_port.find(":");
    if ((split_pos == std::string::npos) || (split_pos + 1 >= ip_port.length()))
    {
        clog_error("parse address failed: %s",addr_cfg.c_str());
        return std::make_pair("", "");
    }

    std::string addr_ip = ip_port.substr(0, split_pos);
    std::string addr_port = ip_port.substr(split_pos + 1);
    int port_tmp = atoi(addr_port.c_str());
    if (port_tmp <= 0 || port_tmp > 0xFFFF)
    {
        clog_error("port in address beyond valid range: %s", addr_cfg.c_str());
    }

    return std::make_pair(addr_ip, addr_port);
}

int32_t LoadDominantContracts(string file, char buffer[][10])
{
	int32_t count = 0;

	std::ifstream is;
	is.open (file);
	string contrs = "";
	if (is) {
		getline(is, contrs);
		contrs += " ";
		size_t start_pos = 0;
		size_t end_pos = 0;
		string contr = "";
		while ((end_pos=contrs.find(" ",start_pos)) != string::npos){
			contr = contrs.substr (start_pos, end_pos-start_pos);
			strcpy(buffer[count], contr.c_str());
			clog_warning("LoadDominantContracts:dominant contract:%s",contr.c_str());

			start_pos = end_pos + 1;
			count++;
		}
	}
	else { clog_error("LoadDominantContracts: can't open: %s", file.c_str()); }

	return count;
}

bool IsDominantImp(char *contract, char buffer[][10], int32_t buffer_size)
{
	bool is_dominant = false;

	for(int i=0; i<buffer_size; i++){
		if(buffer[i][0]==0) break; // hit bottom

		if(IsEqualContract(buffer[i], contract)){
			is_dominant = true;
			break;
		}
	}

	return is_dominant;
}


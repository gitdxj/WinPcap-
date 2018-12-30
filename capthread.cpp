#include "capthread.h"

using namespace std;

capThread::capThread(QObject *parent)
	: QThread(parent)
{
	route_table.add("206.1.1.0","255.255.255.0","0.0.0.0");
	route_table.add("206.1.2.0","255.255.255.0","0.0.0.0");
	route_table.add("206.1.3.0","255.255.255.0","206.1.2.2");
	
	//unsigned char mac1[6];
	//mac1[0] = 0x00;
	//mac1[1] = 0x0c;
	//mac1[2] = 0x29;
	//mac1[3] = 0xea;
	//mac1[4] = 0x2f;
	//mac1[5] = 0x38;
	//arp_table.add("192.168.2.5", mac1);
	//arp_table.add("192.168.1.2", mac1);

	//mac1[0] = 0x00;
	//mac1[1] = 0x0c;
	//mac1[2] = 0x29;
	//mac1[3] = 0x4a;
	//mac1[4] = 0xad;
	//mac1[5] = 0x8a;
	//arp_table.add("192.168.3.3", mac1);

	//mac1[0] = 0x00;
	//mac1[1] = 0x0c;
	//mac1[2] = 0x29;
	//mac1[3] = 0x73;
	//mac1[4] = 0x70;
	//mac1[5] = 0xdd;
	//arp_table.add("192.168.2.1", mac1);
	//arp_table.add("192.168.3.2", mac1);


}

capThread::~capThread()
{

}

void capThread::setPara(pcap_if_t *d)
{
	this->d = d;
}

void capThread::setPara(pcap_if_t * d, std::string srcIP)
{
	this->d = d;
	this->m_src_IP = srcIP;

}

void capThread::setPara(std::string desIP)
{
	this->m_des_IP = desIP;
}

void capThread::run()
{
	mutex.lock();
	pcap_t *adhandle;
	if ( (adhandle= pcap_open(this->d->name,          // name of the device
                              65536,            // portion of the packet to capture. 
                                                // 65536 guarantees that the whole packet will be captured on all the link layers
                              PCAP_OPENFLAG_PROMISCUOUS,    // promiscuous mode
                              1000,             // read timeout
                              NULL,             // authentication on the remote machine
                              errbuf            // error buffer
                              ) ) == NULL)
	{
		exit(1);
	}

	int res;
	struct pcap_pkthdr *header;
	const u_char *pkt_data;
	
	//capture begins
	while((res = pcap_next_ex( adhandle, &header, (&pkt_data))) >= 0)
	{

        if(res == 0)
            /* Timeout elapsed */
            continue;

		ethernet_protocol_packet_callback(NULL, header, pkt_data);

	}
	mutex.unlock();
}


void capThread::ethernet_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content)   
{   
    u_short ethernet_type;   
    /* ��̫������ */   
    struct ether_header *ethernet_protocol;   
    /* ��̫��Э����� */   
    u_char *mac_string;   
    /* ��̫����ַ */   
 
	sprintf(this->m_captureInfo,"%s","**************************************************\n");   
	//emit this->getPacket(this->m_captureInfo);

	//sprintf(this->m_captureInfo,"#%d packet\n", packet_number);   
	//emit this->getPacket(this->m_captureInfo);

    sprintf(this->m_captureInfo,"%s","time: ");
	//emit this->getPacket(this->m_captureInfo);

	struct tm ltime;
	char timestr[16];
	time_t local_tv_sec = packet_header->ts.tv_sec;
    localtime_s(&ltime, &local_tv_sec);
    strftime( timestr, sizeof timestr, "%H:%M:%S", &ltime);

    sprintf(this->m_captureInfo,"%s\n", timestr);  

	//emit this->getPacket(this->m_captureInfo);
    /* ��ò������ݰ���ʱ�� */   


    sprintf(this->m_captureInfo,"%s","packet length: ");   
	//emit this->getPacket(this->m_captureInfo);

    sprintf(this->m_captureInfo,"%d\n", packet_header->len);   
	//emit this->getPacket(this->m_captureInfo);

    sprintf(this->m_captureInfo,"%s","--------   ethernet    --------\n");
	//emit this->getPacket(this->m_captureInfo);

    ethernet_protocol = (struct ether_header*)packet_content;   
    /* �����̫��Э������ */   
    sprintf(this->m_captureInfo,"%s","type: ");  
	emit this->getPacket(this->m_captureInfo);

    ethernet_type = ntohs(ethernet_protocol->ether_type);   
    /* �����̫������ */   
    sprintf(this->m_captureInfo,"%04x\n", ethernet_type);   
    switch (ethernet_type) /* ������̫�������ж� */   
    {   
        case 0x0800:   
            sprintf(this->m_captureInfo,"%s","IP protocal\n");
			emit this->getPacket(this->m_captureInfo);
            break;   
        case 0x0806:   
            sprintf(this->m_captureInfo,"%s","ARP protocal\n");   
			emit this->getPacket(this->m_captureInfo);
            break;   
        case 0x8035:   
            sprintf(this->m_captureInfo,"%s","RARP protocal\n");  
			emit this->getPacket(this->m_captureInfo);
            break;   
        default:   
            break;   
    }   
    sprintf(this->m_captureInfo,"%s","source MAC address: \n");   
	emit this->getPacket(this->m_captureInfo);

    mac_string = ethernet_protocol->ether_shost;   
    sprintf(this->m_captureInfo,"%02x:%02x:%02x:%02x:%02x:%02x\n", *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));   
	emit this->getPacket(this->m_captureInfo);

	if(
		*mac_string == 0x00 &&
		*(mac_string + 1) == 0x0c &&
		*(mac_string + 2) == 0x29 &&
		*(mac_string + 3) == 0x73 &&
		*(mac_string + 4) == 0x70 &&
		*(mac_string + 5) == 0xdd
		)
		return;
   
	/* ���Դ��̫����ַ */   
    sprintf(this->m_captureInfo,"%s","destination MAC address \n");   
	emit this->getPacket(this->m_captureInfo);
   
	mac_string = ethernet_protocol->ether_dhost;   
    sprintf(this->m_captureInfo,"%02x:%02x:%02x:%02x:%02x:%02x\n", *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));   
	emit this->getPacket(this->m_captureInfo);
	
	/* ���Ŀ����̫����ַ */   
    switch (ethernet_type)   
    {   
        case 0x0806:   
            arp_protocol_packet_callback(argument, packet_header, packet_content);   
            break;   
            /* �ϲ�Э��ΪARPЭ�飬���÷���ARPЭ��ĺ�����ע������Ĵ��� */   
        case 0x0800:   
            ip_protocol_packet_callback(argument, packet_header, packet_content);   
            break;   
            /* �ϲ�Э��ΪIPЭ�飬���÷���IPЭ��ĺ�����ע������Ĵ��� */   
        default:   
            break;   
    }   
    printf("**************************************************\n");    
}   

/*
=======================================================================================================================  
������ʵ��ARPЭ������ĺ���������������ص�������ͬ  
=======================================================================================================================  
 */   
void capThread::arp_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content)   
{   



    struct arp_header *arp_protocol;   
    u_short protocol_type;   
    u_short hardware_type;   
    u_short operation_code;   
    u_char *mac_string;   
    struct in_addr source_ip_address;   
    struct in_addr destination_ip_address;   
    u_char hardware_length;   
    u_char protocol_length;   
    printf("--------   ARPЭ��    --------\n");   
    arp_protocol = (struct arp_header*)(packet_content + 14);  

	//cout<<"sizeof packet_header"<<sizeof(packet_content)<<endl;
	//string size = std::to_string((long long)(sizeof(arp_protocol)));
	//this->getMAC(QString::fromStdString(size));

    hardware_type = ntohs(arp_protocol->arp_hardware_type);   
    protocol_type = ntohs(arp_protocol->arp_protocol_type);   
    operation_code = ntohs(arp_protocol->arp_operation_code);   
    hardware_length = arp_protocol->arp_hardware_length;   
    protocol_length = arp_protocol->arp_protocol_length;   
    printf("Ӳ������:%d\n", hardware_type);   
    printf("Э������ Protocol Type:%d\n", protocol_type);   
    printf("Ӳ����ַ����:%d\n", hardware_length);   
    printf("Э���ַ����:%d\n", protocol_length);   
    printf("ARP Operation:%d\n", operation_code);   
    switch (operation_code)   
    {   
        case 1:   
            printf("ARP����Э��\n");   
            break;   
        case 2:   
            printf("ARPӦ��Э��\n");   
            break;   
        case 3:   
            printf("RARP����Э��\n");   
            break;   
        case 4:   
            printf("RARPӦ��Э��\n");   
            break;   
        default:   
            break;   
    }   
    printf("Դ��̫����ַ: \n");   
    mac_string = arp_protocol->arp_source_ethernet_address;   
    //printf("%02x:%02x:%02x:%02x:%02x:%02x\n", *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));   
	sprintf(this->m_captureInfo,"%02x:%02x:%02x:%02x:%02x:%02x", *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));       
	
	unsigned char MAC[6];
	for(int i=0;i<6;i++)
		MAC[i] = *(mac_string+i);

	memcpy((void*) &source_ip_address, (void*) &arp_protocol->arp_source_ip_address, sizeof(struct in_addr));   
    printf("ԴIP��ַ:%s\n", inet_ntoa(source_ip_address));   
    printf("Ŀ����̫����ַ: \n");   
    mac_string = arp_protocol->arp_destination_ethernet_address;   
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n", *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));   
    memcpy((void*) &destination_ip_address, (void*) &arp_protocol->arp_destination_ip_address, sizeof(struct in_addr));   
    printf("Ŀ��IP��ַ:%s\n", inet_ntoa(destination_ip_address));

	string srcMAC = this->m_captureInfo;
	QString QSrcMAC = QString::fromStdString(srcMAC);
	string desIp = inet_ntoa(destination_ip_address);
	string srcIp = inet_ntoa(source_ip_address);
	QString QSrcIP = QString::fromStdString(srcIp);
	//emit getMAC(QSrcIP+":"+QSrcMAC);
	////string ip_addr = this->m_src_IP;
	//if(desIp == this->m_src_IP && srcIp == this->m_des_IP)
	//	emit  getMAC(QSrcMAC);


	//add newly got ip-mac match to arp_table
	this->arp_table.add(srcIp, MAC);
}   

void capThread::tcp_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content){}
void capThread::udp_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content){}
void capThread::icmp_protocol_packet_callback(u_char *argument,const  struct pcap_pkthdr *packet_header, const u_char *packet_content){

	
}

//=======================================================================================================================  
//������ʵ��IPЭ������ĺ������亯��������ص�������ͬ  
//=======================================================================================================================  
// */   
void capThread::ip_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content)   
{   
    struct ip_header *ip_protocol;   
    /* IPЭ����� */   
    u_int header_length;   
    /* ���� */   
    u_int offset;   
    /* ƫ�� */   
    u_char tos;   
    /* �������� */   
    u_int16_t checksum;   
    /* У��� */   
    ip_protocol = (struct ip_header*)(packet_content + 14);   
    /* ���IPЭ������ */   
    checksum = ntohs(ip_protocol->ip_checksum);   
    /* ���У��� */   
    header_length = ip_protocol->ip_header_length *4;   
    /* ��ó��� */   
    tos = ip_protocol->ip_tos;   
    /* ��÷������� */   
    offset = ntohs(ip_protocol->ip_off);   
    /* ���ƫ�� */   
    printf("----------- IPЭ��    -----------\n");   
    printf("�汾��:%d\n", ip_protocol->ip_version);   
    printf("�ײ�����:%d\n", header_length);   
    printf("��������:%d\n", tos);   


    printf("�ܳ���:%d\n", ntohs(ip_protocol->ip_length));    //header + data
	//int total_length = ntohs(ip_protocol->ip_length) + 14; //ip length + ethernet length
	//int total_length = 40;

    printf("��ʶ:%d\n", ntohs(ip_protocol->ip_id));   
    printf("ƫ��:%d\n", (offset &0x1fff) *8);   
    printf("����ʱ��:%d\n", ip_protocol->ip_ttl);   
    printf("Э������:%d\n", ip_protocol->ip_protocol);   
    switch (ip_protocol->ip_protocol)   
    {   
        case 6:   
            printf("�ϲ�Э��ΪTCPЭ��\n");   
            break;   
        case 17:   
            printf("�ϲ�Э��ΪUDPЭ��\n");   
            break;   
        case 1:   
            printf("�ϲ�Э��ΪICMPЭ��ICMP\n");   
            break;   
        default:   
            break;   
    }   
    printf("У���:%d\n", checksum);   
    printf("ԴIP��ַ:%s\n", inet_ntoa(ip_protocol->ip_souce_address));   
	string srcIP=inet_ntoa(ip_protocol->ip_souce_address);
	//this->getMAC(QString::fromStdString("source ip:"+srcIP +"\n"));
    /* ���ԴIP��ַ */   
    printf("Ŀ��IP��ַ:%s\n", inet_ntoa(ip_protocol->ip_destination_address));
	string desIP=inet_ntoa(ip_protocol->ip_destination_address);
	//this->getMAC(QString::fromStdString("destination ip:"+desIP +"\n"));
    /* ���Ŀ��IP��ַ */   
	if(ip_protocol->ip_protocol == 1){

		if("192.168.2.1" == desIP || "192.168.3.2" == desIP)
			return;

		unsigned char packet_content_copy[2000];
		memcpy(packet_content_copy, packet_content, packet_header->len);

		string next;
		if(this->route_table.lookup(desIP, next))
		{
			if("0.0.0.0" == next)
				next = desIP;
			unsigned char next_mac[6];
			char mac_char[100];
			memset(mac_char, 0, 100);
			if(this->arp_table.lookup(next, next_mac))
			{
				this->resend(next_mac, packet_content_copy, packet_header->len);
				sprintf(mac_char,"%02x:%02x:%02x:%02x:%02x:%02x",
					next_mac[0],next_mac[1],next_mac[2],next_mac[3],next_mac[4],next_mac[5]);
				string str_mac = mac_char;
				this->getMAC(QString::fromStdString("des addr: "+ desIP + " "+"forward to MAC addr: " + str_mac + "\n"));
			}
			else {
				this->sendARPFrame(this->d,  next, "192.168.2.1");
				return;
			}
		}
		else return;

	}
    switch (ip_protocol->ip_protocol) /* ����IPЭ���ж��ϲ�Э�� */   
    {   
        case 6:   
            tcp_protocol_packet_callback(argument, packet_header, packet_content);   
            break;   
            /* �ϲ�Э����TCPЭ�飬���÷���TCPЭ��ĺ�����ע������Ĵ��� */   
        case 17:   
            udp_protocol_packet_callback(argument, packet_header, packet_content);   
            break;   
            /* �ϲ�Э����UDPЭ�飬���÷���UDPЭ��ĺ�����ע������Ĵ��� */   
        case 1:   
            icmp_protocol_packet_callback(argument, packet_header, packet_content);   
            break;   
            /* �ϲ�Э����ICMPЭ�飬���÷���ICMPЭ��ĺ�����ע������Ĵ��� */   
        default:   
            break;   
    }   
}   

void capThread::resend(unsigned char *mac, u_char* content, int length)
{
	//change the content according to MAC address
	for(int i=0;i<6;i++)
		*(content + i) = mac[i];

	*(content + 6) = 0x00;
	*(content + 7) = 0x0c;
	*(content + 8) = 0x29;
	*(content + 9) = 0x73;
	*(content + 10) = 0x70;
	*(content + 11) = 0xdd;


	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_t *adhandle;
	if ( (adhandle= pcap_open(this->d->name,          // name of the device
                              65536,            // portion of the packet to capture. 
                                                // 65536 guarantees that the whole packet will be captured on all the link layers
                              PCAP_OPENFLAG_PROMISCUOUS,    // promiscuous mode
                              1000,             // read timeout
                              NULL,             // authentication on the remote machine
                              errbuf            // error buffer
                              ) ) == NULL)
	{
		exit(1);
	}

	if(pcap_sendpacket(adhandle, (u_char*)content, length) != 0)
	{
		//err
	}
	else
	{
		//this->getMAC("resend\n");
	}
}


void capThread::sendARPFrame(pcap_if_t *d, std::string desIp, std::string srcIp)
{
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_t *adhandle;
	if ( (adhandle= pcap_open(this->d->name,          // name of the device
                              65536,            // portion of the packet to capture. 
                                                // 65536 guarantees that the whole packet will be captured on all the link layers
                              PCAP_OPENFLAG_PROMISCUOUS,    // promiscuous mode
                              1000,             // read timeout
                              NULL,             // authentication on the remote machine
                              errbuf            // error buffer
                              ) ) == NULL)
	{
		exit(1);
	}

	unsigned char *desip = new unsigned char[4];
	unsigned char *srcip = new unsigned char[4];
	parseIP(desIp, desip);
	parseIP(srcIp, srcip);

	ARPFrame_t ARPFrame;

	//set Destination MAC address
	ARPFrame.FrameHeader.DesMAC[0] = 0xff;
	ARPFrame.FrameHeader.DesMAC[1] = 0xff;
	ARPFrame.FrameHeader.DesMAC[2] = 0xff;
	ARPFrame.FrameHeader.DesMAC[3] = 0xff;
	ARPFrame.FrameHeader.DesMAC[4] = 0xff;
	ARPFrame.FrameHeader.DesMAC[5] = 0xff;

	//set Source MAC address
	ARPFrame.FrameHeader.SrcMAC[0] = 0x00;
	ARPFrame.FrameHeader.SrcMAC[1] = 0x0c;
	ARPFrame.FrameHeader.SrcMAC[2] = 0x29;
	ARPFrame.FrameHeader.SrcMAC[3] = 0x73;
	ARPFrame.FrameHeader.SrcMAC[4] = 0x70;
	ARPFrame.FrameHeader.SrcMAC[5] = 0xdd;

	ARPFrame.FrameHeader.FramType = htons(0x0806);

	ARPFrame.HardwareType = htons(0x0001);

	ARPFrame.ProtocolType = htons(0x0800);
	ARPFrame.HLen = 6;
	ARPFrame.PLen = 4;

	ARPFrame.Operation = htons(0x0001);

	ARPFrame.SendHa[0] = 0x00;
	ARPFrame.SendHa[1] = 0x0c;
	ARPFrame.SendHa[2] = 0x29;
	ARPFrame.SendHa[3] = 0x73;
	ARPFrame.SendHa[4] = 0x70;
	ARPFrame.SendHa[5] = 0xdd;

	for(int i = 0; i<4; i++)
		ARPFrame.SendIP[i] = srcip[i];

	//ARPFrame.SendIP[0] = 0xc0;
	//ARPFrame.SendIP[1] = 0xa8;
	//ARPFrame.SendIP[2] = 0x8b;
	//ARPFrame.SendIP[3] = 0x81;

	for(int i = 0; i<6; i++)
		ARPFrame.RecvHa[i] = 0x00;

	for(int i = 0; i<4; i++)
	{
		ARPFrame.RecvIP[i] = desip[i];
	}

	if(pcap_sendpacket(adhandle, (u_char *)&ARPFrame, sizeof(ARPFrame_t)) != 0)
	{
		//err
	}
	else
	{

	}
}
// get local ifaces and their MAC address, and compare with VMware as well as VirtualBox OUIs.
// references for getting local net info:
// 1. https://www.cnblogs.com/fnlingnzb-learner/p/6427388.html
// 2. https://blog.csdn.net/fuluoce/article/details/110629284
// 3. https://www.cnblogs.com/quicksnow/p/3299172.html
// VMWare OUI: https://www.cnblogs.com/Solomon-xm/articles/10253575.html
// VirtualBox OUI: https://macaddress.io/faq/how-to-recognise-an-oracle-virtual-machine-by-its-mac-adress

#include "vmware_linux.h"

#define MACFORMAT %02X:%02X:%02X:%02X:%02X:%02X
#define MACLEN 18
#define OUILEN 8
#define MAXIFACES 16

const char* vm_oui[2] = {"00:50:56", "00:0C:29"};
const char* vbox_oui[2] = {"08:00:27", "52:54:00"};

int check_oui(void)
{
	int fd;
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("create socket");
		return -1;
	}

	struct ifconf ifc;
	struct ifreq buf[MAXIFACES];
	int ifn = 0; // total iface numbers

	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = (caddr_t)buf;
	if (ioctl(fd, SIOCGIFCONF, (char*)&ifc)) {
		perror("get iface conf");
		return -1;
	}

	ifn = ifc.ifc_len / sizeof(struct ifreq);
	fprintf(stdout, "total iface number: %d\n", ifn);

	int i = 0;
	char mac[MACLEN];
	while (i < ifn) {
		fprintf(stdout, "iface name: %s\n", buf[i].ifr_name);
		if (!ioctl(fd, SIOCGIFHWADDR, (char*)&buf[i])) {
			memset(mac, 0, sizeof(mac));
			snprintf(mac, MACLEN, "%02X:%02X:%02X:%02X:%02X:%02X",
					(unsigned char)buf[i].ifr_hwaddr.sa_data[0], 
					(unsigned char)buf[i].ifr_hwaddr.sa_data[1], 
					(unsigned char)buf[i].ifr_hwaddr.sa_data[2], 
					(unsigned char)buf[i].ifr_hwaddr.sa_data[3], 
					(unsigned char)buf[i].ifr_hwaddr.sa_data[4], 
					(unsigned char)buf[i].ifr_hwaddr.sa_data[5]);
			fprintf(stdout, "mac addr: %s\n", mac);

			// distinguish virtual environment
			for(int i = 0; i < 2; ++i) {
				if(strncmp(mac, vm_oui[i], OUILEN) == 0)
					fprintf(stdout, "VMWare environment.\n");
				if(strncmp(mac, vbox_oui[i], OUILEN) == 0) 
					fprintf(stdout, "VirtualBox environment.\n");
			}
		}
		++i;
	}
	return 0;
}


int check_vmcpu(void)
{
    FILE * fp;
    char *p = "VMware";
    char buf[1024];
    memset(buf, '\0', sizeof(buf));
    if(NULL == (fp = popen("lscpu", "r"))) {     
        fprintf(stderr, "execute command failed: %s", strerror(errno));      
        return -1;      
    }   
    while(NULL != fread( buf, sizeof(char), sizeof(buf), fp)) {  
		printf("%s", buf);    
		if(strstr(buf, p)) {
			printf("this is vmware\n");
		}
		else {
			printf("this is not vmware\n");
		}
    }  
    pclose(fp);
    return 0;   
}


int check_vmtool(void)
{
    FILE * fp;
    char buffer[80];
    if(NULL == (fp = popen("vmware-toolbox-cmd -v", "r"))) {     
        fprintf(stderr, "execute command failed: %s", strerror(errno));      
        return -1;      
    }   
    while(NULL != fgets(buffer, sizeof(buffer), fp)) {  
		printf("%s", buffer); 
		if(buffer[0] >= '0' && buffer[0] <= '9') {
			printf("this is vmware\n");
		}
        else {
			printf("this is not vmware\n");
		}
    }  

    pclose(fp);
    return 0; 
}


/* only used for test */
int main(int argc, char* argv[])
{
	check_oui();
	check_vmcpu();
	check_vmtool();
	return 0;
}

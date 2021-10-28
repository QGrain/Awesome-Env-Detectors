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
	int ck_result = 0;
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("create socket");
		return 0;
	}

	struct ifconf ifc;
	struct ifreq buf[MAXIFACES];
	int ifn = 0; // total iface numbers

	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = (caddr_t)buf;
	if (ioctl(fd, SIOCGIFCONF, (char*)&ifc)) {
		perror("get iface conf");
		return 0;
	}

	ifn = ifc.ifc_len / sizeof(struct ifreq);
	// fprintf(stdout, "total iface number: %d\n", ifn);

	int i = 0;
	char mac[MACLEN];
	while (i < ifn) {
		// fprintf(stdout, "iface name: %s\n", buf[i].ifr_name);
		if (!ioctl(fd, SIOCGIFHWADDR, (char*)&buf[i])) {
			memset(mac, 0, sizeof(mac));
			snprintf(mac, MACLEN, "%02X:%02X:%02X:%02X:%02X:%02X",
					(unsigned char)buf[i].ifr_hwaddr.sa_data[0], 
					(unsigned char)buf[i].ifr_hwaddr.sa_data[1], 
					(unsigned char)buf[i].ifr_hwaddr.sa_data[2], 
					(unsigned char)buf[i].ifr_hwaddr.sa_data[3], 
					(unsigned char)buf[i].ifr_hwaddr.sa_data[4], 
					(unsigned char)buf[i].ifr_hwaddr.sa_data[5]);
			// fprintf(stdout, "mac addr: %s\n", mac);

			// distinguish virtual environment
			for(int i = 0; i < 2; ++i) {
				if(strncmp(mac, vm_oui[i], OUILEN) == 0) {
					// fprintf(stdout, "VMWare environment.\n");
					ck_result = 1;
					goto Exit;
				}
				if(strncmp(mac, vbox_oui[i], OUILEN) == 0) {
					// fprintf(stdout, "VirtualBox environment.\n");
					ck_result = 2;
					goto Exit;
				}
			}
		}
		++i;
	}

Exit:
	return ck_result;
}


int check_vmcpu(void)
{
    FILE * fp;
    char *p = "VMware";
    char buf[1280];
	int ck_result = 0;
    memset(buf, 0, sizeof(buf));

    if(NULL == (fp = popen("lscpu", "r"))) {
        fprintf(stderr, "execute lscpu failed: %s", strerror(errno));  
        return 0;
    }

    int ret = fread(buf, sizeof(char), sizeof(buf), fp);
	if(strstr(buf, p)) {
		ck_result = 1;
		goto Exit;
	}

Exit:
    pclose(fp);
    return ck_result;
}


int check_vmtool(void)
{
    FILE * fp;
    char buffer[80];
	int ck_result = 0;

    if(NULL == (fp = popen("vmware-toolbox-cmd -v", "r"))) {
        fprintf(stderr, "execute vmware-toolbox-cmd failed: %s", strerror(errno));
        return 0;
    }

    char *ret = fgets(buffer, sizeof(buffer), fp);
	if(buffer[0] >= '0' && buffer[0] <= '9') {
		// printf("this is vmware\n");
		ck_result = 1;
		goto Exit;
	}  

Exit:
    pclose(fp);
    return ck_result; 
}


int CkVmLinuxPrint()
{
	int confidence = 0;
    if(check_oui() == 1) {
        printf("[*] %-50s%10s\n","MAC Address check","[ BAD  ]");
		confidence += 1;
	}
    else {
        printf("[*] %-50s%10s\n","MAC Address check","[ GOOD ]");
	}

    if(check_vmcpu() == 1) {
        printf("[*] %-50s%10s\n","VM CPU check(VMware)","[ BAD  ]");
		confidence += 1;
	}
	else if(check_vmcpu() == 2) {
        printf("[*] %-50s%10s\n","VM CPU check(VirtualBox)","[ BAD  ]");
		confidence += 1;
	}
    else {
        printf("[*] %-50s%10s\n","VM CPU check","[ GOOD ]");
	}

    if(check_vmtool() == 1) {
        printf("[*] %-50s%10s\n","VMware Tools check","[ BAD  ]");
		confidence += 1;
	}
    else {
        printf("[*] %-50s%10s\n","VMware Tools check","[ GOOD ]");
	}

	// confidence belong to [0, 3]
	return confidence;
}


/* only used for test */
// int main(int argc, char* argv[])
// {
// 	CkVmLinuxPrint();
// 	return 0;
// }

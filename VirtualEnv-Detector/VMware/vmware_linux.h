#ifndef VMWARE_LINUX_H
#define VMWARE_LINUX_H

/* Used for detecting Linux virtual environment.*/

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

int check_oui(void);
int check_vmcpu(void);
int check_vmtool(void);
void CkVmLinuxPrint();

#endif

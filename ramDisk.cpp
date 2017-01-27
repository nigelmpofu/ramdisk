/*
* File: ramDisk.cpp
* Author: Nigel Mpofu (nvmpofu@gmail.com)
* Last Edit: 23 January 2017
*
* This is used to mount a ramdisk that the user can access for greater access speeds
* Files shall be lost when the computer loses power or the drive is unmounted
* Note: Must be run with super user privileges. 
* Default: 1GB of storage is allocated
*
* TODO: Variable Drive Sizes
* TODO: Allow for other mount points
* TODO: Safety Checks - Enough free memory
* TODO: Create symlink for easier access
*/

#include <iostream>
#include <cstring>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h> // getuid

using namespace std;

/* Function Prototpyes */
bool mount(int = 1024);
bool unmount();
bool checkMount();
void help();
void version();

/* Constant Variables */
const char MOUNT_STR[] = "-mount";
const char UNMOUNT_STR[] = "-unmount";
const char HELP_STR[] = "-help";
const char VERSION_STR[] = "-version";
const int VERSION_M = 1;
const int VERSION_REV = 1;
const char AUTH_NAME[] = "Nigel Mpofu";
const char AUTH_EMAIL[] = "nvmpofu@gmail.com";

int main(int argc, char** argv)
{
	// Check for root access
	if(getuid() != 0)
	{
		cout << "Error: Root Access Required" << endl;
		return 2;
	}
	// Check argument number
	if(argc < 2)
	{
		cout << "Error: Too few arguments" << endl;
		return 1;
	}
	if(argc > 2)
	{
		cout << "Error: Too many arguments" << endl;
		return 1;
	}

	/* Check arguments for what to do */
	if(strcmp(MOUNT_STR, argv[1]) == 0) // Mount
	{
		cout << "Mounting...\n";
		if(!mount())
		{
			cout << "Error: Mounting Failed" << endl;
			return 1;
		}
		else
		{
			cout << "Successfully Mounted" << endl;
			return 0;
		}
	}
	else if(strcmp(UNMOUNT_STR, argv[1]) == 0) // Unmount
	{
		cout << "Unmounting...\n";
		if(!unmount())
		{
			cout << "Error: Unmounting Failed" << endl;
			return 1;
		}
		else
		{
			cout << "Successfully Unmounted" << endl;
			return 0;
		}
	}
	else if(strcmp(VERSION_STR, argv[1]) == 0) // Version
	{
		version();
		return 0;
	}
	else if(strcmp(HELP_STR, argv[1]) == 0) // Help
	{
		help();
		return 0;
	}
	else // Display help on usage error
	{
		cout << "Invalid Command: " << argv[1] << endl;
		return 1;
	}
}

/* Check if mount point already in use */
bool checkMount()
{
	ifstream file;
	string line;
	int offset;
	char search[16] = "/media/ramdisk";
	bool isFound = false;
	file.open("/etc/mtab");
	if(file.is_open())
	{
		while(!file.eof())
		{
			getline(file, line);
			if((offset = line.find(search, 0)) != string::npos)
			{
				// found
				isFound = true;
				break;
			}
		}
		file.close();
	}
	return isFound;
}

/* Function to mount the ramdisk to '/media/ramdisk' */
bool mount(int sizeM)
{
	/*
	* TODO: Variable disk sizes
	* TODO: Allow for other mount points
	*/

	// Check if the mount point exists
	struct stat mp;
	if(stat("/media/ramdisk", &mp) == 0)
	{
		if(mp.st_mode & S_IFDIR == 0)
		{
			// Directory Not Found, attempt to create it
			if(mkdir("/media/ramdisk", 0777) != 0)
			{
				// Mount point creation error
				cout << "Error: Could not create mount point" << endl;
				return false;
			}
		}
	}
	else
	{
		cout << "Error: Mount Point Check Failure" << endl;
		perror("stat");
		return false;
	}

	// Check if mount point is available
	if(checkMount())
	{
		cout << "Error: Mount point already in use" << endl;
		return false;
	}

	// Attempt to mount the ram drive
	char cmd[64] = "mount -t tmpfs -o size=1024m tmpfs /media/ramdisk";
	if(system(cmd) != 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

/* Function to unmount the ramdisk */
bool unmount()
{
	/* Attempt to wipe drive to free memory before unmounting */
	char wipecmd[32] = "rm -rf /media/ramdisk/*"; // WARNING: DATA WIPE (Garbage Collection)
	if(system(wipecmd) != 0)
	{
		cout << "Aborting: Drive not empty" << endl;
		return false;
	}

	// Attempt to unmount drive
	char cmd[32] = "umount /media/ramdisk";
	if(system(cmd) != 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}
 /* Displays usage iformation */
void help()
{
	cout << "*** RamDisk Available Commands ***\n";
	cout << "-mount   - Mounts the 1GB (1024MB) Ram Disk to '/media/ramdisk'\n";
	cout << "-unmount - Unmounts the Ram Disk\n";
	cout << "Note: Data is lost on unmount and system power loss.\n\n";
	version();
}

/* Displays the version number and author information */
void version()
{
	cout << "RamDisk Version: " << VERSION_M << "." << VERSION_REV << endl;
	cout << "By: " << AUTH_NAME << " (" << AUTH_EMAIL << ")" << endl;
}
#include "test.h"

void get_version(void)
{
	get_ver("\n"
"   	Create date is %s\n"
	"	Build number is %s\n"
			, TOOL_DATE, TOOL_VERSION);
	exit(2);
}

void usage(void)
{
	get_usage("[--gpio agrv] [--rs232 agrv] [--rs485 agrv] [--display agrv] [--version]\n"
"   		[-a -b agrv] [-c agrv] [-d agrv]\n"
"example:\n"
"	--gpio		gpio test\n"
"	--rs232		rs232 test\n"
"	--rs485		rs485 test\n"
"	--display	video out test\n"
"	-v|--version	get version\n"
);
	exit(2);
}

void usage_gpio(void)
{
	get_usage("--gpio in|out|pair num1 num2 num3 num4 .....(max.%d)\n"
"example:\n"
"	--gpio in 13 33 88 133 44\n"
"	--gpio out 22 33 25 66 77 111 68\n"
"	--gpio pair 13 44 55 66 33 101\n"
	, ARRAY_MAX);
	exit(2);
}

void usage_rs232(void)
{
	get_usage("--rs232 client|server|repeat port_node\n"
"example:\n"
"	--rs232 client /dev/ttymxc0\n"
"	--rs232 server /dev/ttymxc0\n"
"	--rs232 repeat /dev/ttymxc2\n"
);
	exit(2);
}

/*startup_func
 * 	for startup test
 *      --startup ./test.txt
 */  
int startup_func(char *path)
{
	int ret;
	int count = 0;
	char str[128];
	char result_buf[256];
	FILE *fp;
	
	fp = fopen(path, "r");
	if( fp == NULL ){
		goto create;
	}
	while(fgets(result_buf, sizeof(result_buf), fp) != NULL){
		sscanf(result_buf, "%s%d", str, &count);
		debug_war("result_buf : %s, %s, %d\n", result_buf, str, count) ;
	}
	ret = fclose(fp);
	if( fp == NULL ){
		return 128;
	}

create:
	fp = fopen(path, "w+");
	count++;
	fprintf(fp, "starupcount= %d\n", count);
	ret = fclose(fp);
	if( fp == NULL ){
		return 128;
	}
	
	debug_eng("startup times : %d\n", count) ;
	return 0;
}

/*main(int argc, char **argv)
 * 	
 *       
 */  
int main(int argc, char **argv)
{
	int ch;
	int long_index = 0;
	int ret = 0;
	
	debug_veb("arc: %d, argv[1]: %s, argv[2]: %s\n", argc, argv[1], argv[2]);
	while ((ch = getopt_long(argc, argv, COMMON_SHORT_OPTSTR, COMMON_LONG_OPTSTR, &long_index)) != EOF) {
		switch(ch) {
		case 0:	//gpio
			{
				int nums=argc-3;
				int pin_num[ARRAY_MAX];		
				int iCount = 0 ;
				get_usage("0 = gpio\n");
				for( iCount = 3; iCount < argc ; iCount++)
				{
					pin_num[iCount-3] = strtol(argv[iCount] ,NULL, 0);
				}

				argv++;
				if (strcmp( argv[1], "in") == 0){
					ret = 0 ;
				}else if (strcmp( argv[1], "out") == 0){
					ret = 1 ;					
				}else if (strcmp( argv[1], "pair") == 0){
					ret = 2 ;					
				}else{
					usage_gpio() ;	
				}
				return ret ;
			break ;
			}
			
		case 1: //rs232
				get_usage("rs232 = 1\n");
				argv++ ;
				if (strcmp( argv[1], "client") == 0){
					ret = 0 ;				
				}else{
					usage_rs232() ;	
				}
				return ret ;
			break ;
			
		case 3: //startup
				get_usage("startup = 1\n");
				argv++ ;
				ret = startup_func(argv[1]) ;
				return ret ;
			break ;
			
		case 4: //display
				get_usage("display = 4\n");
			break ;
		case 'd':
				get_usage("uuu = d\n");
			{
			break ;
			}
			
		case 'v': //get version
		{
			get_usage("version = v\n");
			get_version();	
			break ;
		}

		case 'z': //test
		{
			get_usage("test = z\n");
			int i1, i2, i3, i4 ;
			if (sscanf(optarg, "%u.%u.%u.%u", &i1, &i2, &i3, &i4) == 4) {
				debug_msg("%d,%d,%d,%d\n", i1, i2, i3, i4) ;
			}	
			break ;
		}

		default:
		{
			get_usage("default - -\n");
			usage() ;
		}
		}
	}

	return 0 ;
}


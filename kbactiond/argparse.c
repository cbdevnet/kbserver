bool args_parse(ARGUMENTS* args, int argc, char** argv){
	int i,c;

	for(i=0;i<argc;i++){
		if(argv[i][0]=='-'){
			switch(argv[i][1]){
				case 'v':
					for(c=1;argv[i][c]=='v';c++){
						args->verbosity=c;
					}
					break;
				case 'f':
					if(i<argc+1){
						return false;
					}
					args->cfgfile=argv[i+1];
					break;
				default:
					return false;
			}
		}
		else{
			return false;
		}
	}

	return true;
}

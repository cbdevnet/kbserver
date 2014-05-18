int logic_process_incoming(ARGUMENTS* args, CONFIG* cfg){
	unsigned i;

	for(i=0;cfg->inputs[i];i++){
		if(cfg->inputs[i]->active&&cfg->inputs[i]->data_offset>0){
			//check timeout
			//longest-match tokenizing
			//resolve to command/action
			//fill cmd_buf
			//execute
			//update timestamp
		}
	}
}

int logic_process_incoming(ARGUMENTS* args, CONFIG* cfg){
	unsigned i;
	time_t current_time;

	for(i=0;cfg->inputs[i];i++){
		if(cfg->inputs[i]->active&&cfg->inputs[i]->data_offset>0){
			current_time=time(NULL);

			//check timeout
			if(current_time-cfg->inputs[i]->last_event>cfg->conn_timeout){
				cfg->inputs[i]->data_offset=0;
			}

			//longest-match tokenizing
			//TODO

			//resolve to command/action
			//fill cmd_buf
			//execute
			//TODO
			
			//update timestamp
			cfg->inputs[i]->last_event=current_time;

			//clear overfull buffers
			if(sizeof(cfg->inputs[i]->data_buf)-1-cfg->inputs[i]->data_offset<1){
				cfg->inputs[i]->data_offset=0;
			}
		}
	}

	return 0;
}

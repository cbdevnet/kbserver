int logic_process_incoming(ARGUMENTS* args, CONFIG* cfg){
unsigned i, head_off=0, tail_off, c;
	time_t current_time;
	char char_buf;
	TOKEN_TYPE token_type=T_INVALID;
	TOKEN* token;

	for(i=0;cfg->inputs[i];i++){
		if(cfg->inputs[i]->active&&cfg->inputs[i]->data_offset>0){
			if(args->verbosity>3){
				fprintf(stderr, "Processing data on connection %d (%d bytes)\n", i, cfg->inputs[i]->data_offset);
			}
			current_time=time(NULL);

			//check timeout
			if(current_time-cfg->inputs[i]->last_event>cfg->conn_timeout){
				//command_offset=0
			}

			while(cfg->inputs[i]->data_offset>0&&token_type!=T_INCOMPLETE){
				//longest match tokenizing
				for(tail_off=cfg->inputs[i]->data_offset;head_off<cfg->inputs[i]->data_offset;tail_off--){
					//check for match between head_off and tail_off
					char_buf=cfg->inputs[i]->data_buf[tail_off];
					cfg->inputs[i]->data_buf[tail_off]=0;
					token=token_resolve(args, cfg, &token_type, cfg->inputs[i]->data_buf+head_off);
					cfg->inputs[i]->data_buf[tail_off]=char_buf;

					//FIXME if incomplete & tail_off!=end, kill anyway

					if(tail_off-1<=head_off){
						tail_off=cfg->inputs[i]->data_offset;
						head_off++;
					}
				}

				if(head_off>0){
					//strip head_off bytes from head
					for(c=0;c<cfg->inputs[i]->data_offset-head_off;c++){
						cfg->inputs[i]->data_buf[c]=cfg->inputs[i]->data_buf[c+head_off];
					}

					//update data_offset
					cfg->inputs[i]->data_offset-=head_off;
				}

				if(token_type!=T_INCOMPLETE){
					//kill detected token
					//update data_offset
					//TODO
				}

				//resolve to command/action
				//fill cmd_buf
				//execute
				//TODO
			}
			
			//update timestamp
			cfg->inputs[i]->last_event=current_time;

			//clear overfull buffers
			if(sizeof(cfg->inputs[i]->data_buf)-1-cfg->inputs[i]->data_offset<1){
				cfg->inputs[i]->data_offset=0;
			}

			if(args->verbosity>3){
				fprintf(stderr, "Processing of connection %d left %d bytes in buffer\n", i, cfg->inputs[i]->data_offset);
			}
		}
	}

	return 0;
}

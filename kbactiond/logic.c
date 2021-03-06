int logic_run_command(ARGUMENTS* args, char* command){
	char* exec_args[]={EXECUTOR, EXECUTOR_FLAG, command, NULL};
	switch(fork()){
		case 0:
			//child
			if(args->verbosity>2){
				fprintf(stderr, "Running \"%s\" in child\n", command);
			}
			execvp(exec_args[0], exec_args);
			perror("execvp");
			exit(1);

		case -1:
			perror("fork");
			break;

		default:
			return 0;
	}

	return -1;
}

int logic_process_incoming(ARGUMENTS* args, CONFIG* cfg){
	unsigned i, head_off=0, tail_off, c;
	time_t current_time;
	char char_buf;
	TOKEN_TYPE token_type=T_NOMATCH;
	TOKEN* token;

	for(i=0;cfg->inputs[i];i++){
		if(cfg->inputs[i]->active&&cfg->inputs[i]->data_offset>0){
			if(args->verbosity>3){
				fprintf(stderr, "Processing data on connection %d (%d bytes)\n", i, cfg->inputs[i]->data_offset);
			}
			current_time=time(NULL);

			//check timeout
			if(current_time-cfg->inputs[i]->last_event>cfg->conn_timeout){
				cfg->inputs[i]->cmd_buf[0]=0;
			}

			while(cfg->inputs[i]->data_offset>0&&token_type!=T_INCOMPLETE){
				//longest match tokenizing
				for(tail_off=cfg->inputs[i]->data_offset;head_off<cfg->inputs[i]->data_offset;tail_off--){
					//check for match between head_off and tail_off
					char_buf=cfg->inputs[i]->data_buf[tail_off];
					cfg->inputs[i]->data_buf[tail_off]=0;
					token=token_resolve(args, cfg, &token_type, cfg->inputs[i]->data_buf+head_off);
					cfg->inputs[i]->data_buf[tail_off]=char_buf;

					//if incomplete & tail_off!=end, kill anyway
					if(token_type==T_INCOMPLETE&&tail_off<cfg->inputs[i]->data_offset){
						if(args->verbosity>3){
							fprintf(stderr, "Incomplete match at invalid position\n");
						}
						token_type=T_NOMATCH;
					}

					if(token_type!=T_NOMATCH){
						if(args->verbosity>2){
							fprintf(stderr, "Resolved %d-%d to \"%s\" (%s)\n", head_off, tail_off, (token->command)?(token->command):(token->token), dbg_token_type(token->type));
						}
						break;
					}

					if(tail_off-1<=head_off){
						tail_off=cfg->inputs[i]->data_offset+1;
						head_off++;
					}
				}

				if(head_off>0){
					//strip head_off bytes from head
					if(args->verbosity>3){
						fprintf(stderr, "Stripping %d bytes off head, data_offset is %d\n", head_off, cfg->inputs[i]->data_offset);
					}

					for(c=0;c<cfg->inputs[i]->data_offset-head_off;c++){
						cfg->inputs[i]->data_buf[c]=cfg->inputs[i]->data_buf[c+head_off];
					}

					//update data_offset
					cfg->inputs[i]->data_offset-=head_off;

					head_off=0;
				}

				if(cfg->inputs[i]->data_offset>0){
					if(token_type!=T_INCOMPLETE){

						//kill detected token
						for(c=0;c<cfg->inputs[i]->data_offset-strlen(token->token);c++){
							cfg->inputs[i]->data_buf[c]=cfg->inputs[i]->data_buf[c+strlen(token->token)];
						}

						//update data_offset
						cfg->inputs[i]->data_offset-=strlen(token->token);

						if(token_type!=T_EXEC){
							//fill cmd_buf
							if(token->command[0]>0){
								switch(token_type){
									case T_START:
										strncpy(cfg->inputs[i]->cmd_buf, token->command, sizeof(cfg->inputs[i]->cmd_buf)-2);
										strncat(cfg->inputs[i]->cmd_buf, " ", 1);
										break;

									case T_APPEND:
										if(cfg->inputs[i]->cmd_buf[0]==0){
											break;
										}
									case T_DO:
										strncat(cfg->inputs[i]->cmd_buf, token->command, sizeof(cfg->inputs[i]->cmd_buf)-1-strlen(cfg->inputs[i]->cmd_buf));
										break;

									case T_PARAM:
										if(sizeof(cfg->inputs[i]->cmd_buf)-1-strlen(cfg->inputs[i]->cmd_buf)>1){
											//add space
											strncat(cfg->inputs[i]->cmd_buf, " ", 1);

											//insert parameter
											strncat(cfg->inputs[i]->cmd_buf, token->command, sizeof(cfg->inputs[i]->cmd_buf)-1-strlen(cfg->inputs[i]->cmd_buf));
										}
										break;

									default:
										break;
								}
							}
						}
						else{
							//direct exec
							//TODO check return value
							logic_run_command(args, token->command);
						}

						//execute
						if(token_type==T_DO&&cfg->inputs[i]->cmd_buf[0]>0){
							//TODO check return value
							logic_run_command(args, cfg->inputs[i]->cmd_buf);
							cfg->inputs[i]->cmd_buf[0]=0;
						}

					}
					else{
						if(args->verbosity>2){
							fprintf(stderr, "Incomplete token in %d bytes (\"%s\")\n", cfg->inputs[i]->data_offset, cfg->inputs[i]->data_buf);
						}
					}
				}
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

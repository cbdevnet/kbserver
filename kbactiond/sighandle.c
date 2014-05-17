void sig_interrupt(int param){
	//handle interrupt gracefully
	stop_processing=true;
	
	printf("Should shut down gracefully now...\n");
}

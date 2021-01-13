int main(void){
	char buffer[1024];
	int i = 0;
	for(; i < 1023; i ++){
		buffer[i] = 'A';
	}
	buffer[1023] = '\0'; 

	printf("%s",buffer);
	return 0;
}

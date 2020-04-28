Client side:
	int sfd = socket(<Address Family>, <transport/socket protocol>, <options>)
 
 	(Note: getaddrinfo() is currently preferred .. but ... eehh... way too much work to get way more than I need)
	
		// get IP address/host name translated in to some useful format
		struct hostent* result = gethostbyname(<some addr or hostname>);
		
		// make the super-special struct that C requires for connect()
		struct sockaddr_in serverAddress;

		// zero it out
		bzero(&serverAddress, sizeof(serverAddress));

		// set the addr family (i.e. Internet)
		serverAddress.sin_family = AF_INET;
 
		// set port number to connect to ... also, convert my numeric type to a general 'network short' type
		serverAddress.sin_port = htons(7621);

		// copy the RAW BYTES! (ick!) from the hostent struct in to the sockaddr_in one
		bcopy( (char*)result->h_addr, (char*)&serverAddress.sin_addr.s_addr, result->addr_length   );
 
 
	connect(sfd, &serverAddress, sizeof(serverAddress)  );
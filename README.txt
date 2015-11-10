									

Unicast-Package

Byte    		0			1			2			3			4...
Description		Source-ID	Dest-ID		0 = Search	Hopcount	Path
										1 = Alive	Hopcount	Path (copied from Search)
				Status					2 = Config	Hopcount    Path 
										
Broadcast-Package

Byte    		0			1			2			3			4			5			6		
Description		Source-ID	Dest-ID		3 = FAlarm	MSG-ID		Sensortype	Distance-H	Distance-L 
										4 = FSearch MSG-ID
										5 = FAck	SUM-RSSI	Path
	
Qt-Package
Byte    		0			1			2			3			4			5			6	
Description		Mote-ID		ROUTE_OK	RSSI-SUM	Path ...					
				Mote-ID		Distance-H	Distance-L
				ID or ID+100


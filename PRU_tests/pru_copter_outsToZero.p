// Demonstrates using Enhanced GPIO (EGP), the fast way to  
 // do GPIO on certain pins with a PRU.  
 //  
 // Writing to r30 or reading from r31 with PRU0 or PRU1 sets or reads the pins  
 // given in this table:  
 // http://elinux.org/Ti_AM33XX_PRUSSv2#Beaglebone_PRU_connections_and_modes  
 //  
 // But only if the Pinmux Mode has been set correctly with a device  
 // tree overlay!  
 //  
 // Assemble with:  
 // pasm -b pru_copter_outsToZero.p  
 // Run with:
 // sudo ./pru_loader pru_copter_outsToZero.bin
   
 // Boilerplate  
 .origin 0  
 .entrypoint TOP  
   
 TOP:  
  
  clr r30, r30, 4
  clr r30, r30, 6  
  clr r30, r30, 14 
  clr r30, r30, 15  
   
 
  // Interrupt the host so it knows we're done  
  mov r31.b0, 19 + 16  
   
 // Don't forget to halt or the PRU will keep executing and probably  
 // require rebooting the system before it'll work again!  
 halt  

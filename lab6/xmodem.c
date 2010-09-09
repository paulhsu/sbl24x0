#define PACKET_SIZE	128

/* Line control codes */
#define SOH			0x01	/* start of header */
#define ACK			0x06	/* Acknowledge */
#define NAK			0x15	/* Negative acknowledge */
#define CAN			0x18	/* Cancel */
#define EOT			0x04	/* end of text */

/*
 * int GetRecord(char , char *)
 *  This private function receives a x-modem record to the pointer and
 * returns non-zero on success.
 */
static int
GetRecord(char blocknum, char *dest)
{
	int		size;
	int		ch;
	unsigned	chk, j, error;

	chk = 0;
	error=0;
retry:	
	if ((ch = getchar(1)) == -1)
		goto err;
	if (ch != blocknum)
		goto err;
	if ((ch = getchar(1)) == -1)
		goto err;
	if (ch != (~blocknum & 0xff))
		goto err;

	for (size = 0; size < PACKET_SIZE; ++size) {
		if ((ch = getchar(1)) == -1)
			goto err;
		chk = chk ^ ch << 8;
		for (j = 0; j < 8; ++j) {
			if (chk & 0x8000)
				chk = chk << 1 ^ 0x1021;
			else
				chk = chk << 1;
		}
		*dest++ = ch;
	}

	chk &= 0xFFFF;

	if (((ch = getchar(1)) == -1) || ((ch & 0xff) != ((chk >> 8) & 0xFF)))
	    goto err;
	if (((ch = getchar(1)) == -1) || ((ch & 0xff) != (chk & 0xFF)))
	    goto err;
	putc(ACK);

	return (1);
err:;
	putc(CAN);
	if(error >3) return(0); else goto retry;
	// We should allow for resend, but we don't.
	return (0);
}

/*
 * int xmodem_rx(char *)
 *  This global function receives a x-modem transmission consisting of
 * (potentially) several blocks.  Returns the number of bytes received or
 * -1 on error.
 */
int
xmodem_rx(char *dest)
{
	int		starting, ch;
	char		packetNumber, *startAddress = dest;

	packetNumber = 1;
	starting = 1;

	while (1) {
		if (starting)
			putc('C');
		if (((ch = Getc(100)) == -1) || (ch != SOH && ch != EOT))
			continue;
		if (ch == EOT) {
			putc(ACK);
			return (dest - startAddress);
		}
		starting = 0;
		// Xmodem packets: SOH PKT# ~PKT# 128-bytes CRC16
		if (!GetRecord(packetNumber, dest))
			return (-1);
		dest += PACKET_SIZE;
		packetNumber++;
	}

	// the loop above should return in all cases
	return (-1);
}

/*	uipc_proto.c	4.4	81/11/16	*/

#include "../h/param.h"
#include "../h/socket.h"
#include "../h/protocol.h"
#include "../h/protosw.h"
#include "../h/mbuf.h"
#include "../net/inet.h"

/*
 * Protocol configuration table and routines to search it.
 *
 * SHOULD INCLUDE A HEADER FILE GIVING DESIRED PROTOCOLS
 */

/*
 * Local protocol handler.
 */
int	pi_usrreq();

/*
 * TCP/IP protocol family: IP, ICMP, UDP, TCP.
 */
int	ip_input(),ip_output();
int	ip_init(),ip_slowtimo(),ip_drain();
int	icmp_input();
int	icmp_drain();
int	udp_input(),udp_ctlinput();
int	udp_usrreq(),udp_sense();
int	udp_init();
int	tcp_input(),tcp_ctlinput();
int	tcp_usrreq(),tcp_sense();
int	tcp_init(),tcp_fasttimo(),tcp_slowtimo(),tcp_drain();
int	ri_input(),ri_ctlinput();
int	ri_usrreq(),ri_sense();

struct protosw protosw[] = {
{ SOCK_STREAM,	PF_LOCAL,	0,		PR_CONNREQUIRED,
  0,		0,		0,		0,
  pi_usrreq,	0,		0,
  0,		0,		0,		0,
},
{ SOCK_DGRAM,	PF_LOCAL,	0,		PR_ATOMIC|PR_ADDR,
  0,		0,		0,		0,
  pi_usrreq,	0,		0,
  0,		0,		0,		0,
},
{ SOCK_RDM,	PF_LOCAL,	0,		PR_ATOMIC|PR_ADDR,
  0,		0,		0,		0,
  pi_usrreq,	0,		0,
  0,		0,		0,		0,
},
{ SOCK_RAW,	PF_LOCAL,	0,		PR_ATOMIC|PR_ADDR,
  0,		0,		0,		0,
  pi_usrreq,	0,		0,
  0,		0,		0,		0,
},
{ 0,		0,		0,		0,
  ip_input,	ip_output,	0,		0,
  0,		0,		0,
  ip_init,	0,		ip_slowtimo,	ip_drain,
},
{ 0,		0,		IPPROTO_ICMP,	0,
  icmp_input,	0,		0,		0,
  0,		0,		0,
  0,		0,		0,		icmp_drain,
},
{ SOCK_DGRAM,	PF_INET,	IPPROTO_UDP,	PR_ATOMIC|PR_ADDR,
  udp_input,	0,		udp_ctlinput,	0,
  udp_usrreq,	udp_sense,	MLEN,
  udp_init,	0,		0,		0,
},
{ SOCK_STREAM,	PF_INET,	IPPROTO_TCP,	PR_CONNREQUIRED,
  tcp_input,	0,		tcp_ctlinput,	0,
  tcp_usrreq,	tcp_sense,	MLEN,
  tcp_init,	tcp_fasttimo,	tcp_slowtimo,	tcp_drain,
},
{ SOCK_RAW,	PF_INET,	IPPROTO_RAW,	PR_ATOMIC|PR_ADDR,
  ri_input,	0,		ri_ctlinput,	0,
  ri_usrreq,	ri_sense,	MLEN,
  0,		0,		0,		0,
}
};

#define	NPROTOSW	(sizeof(protosw) / sizeof(protosw[0]))

struct	protosw *protoswLAST = &protosw[NPROTOSW-1];

/*
 * Operations on protocol table and protocol families.
 */

/*
 * Initialize all protocols.
 */
pfinit()
{
	register struct protosw *pr;

	for (pr = protoswLAST; pr >= protosw; pr--)
		if (pr->pr_init)
			(*pr->pr_init)();
}

/*
 * Find a standard protocol in a protocol family
 * of a specific type.
 */
struct protosw *
pffindtype(family, type)
	int family, type;
{
	register struct protosw *pr;

	if (family == 0)
		return (0);
	for (pr = protosw; pr < protoswLAST; pr++)
		if (pr->pr_family == family && pr->pr_type == type)
			return (pr);
	return (0);
}

/*
 * Find a specified protocol in a specified protocol family.
 */
struct protosw *
pffindproto(family, protocol)
	int family, protocol;
{
	register struct protosw *pr;

	if (family == 0)
		return (0);
	for (pr = protosw; pr < protoswLAST; pr++)
		if (pr->pr_family == family && pr->pr_protocol == protocol)
			return (pr);
	return (0);
}

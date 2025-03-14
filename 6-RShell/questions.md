1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

It calls `recv()` function in a loop until it receives an EOF character.

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

When sending data over TCP, it can arrive in chunks over multiple `recv()` calls. To preserve message boundaries, the shell should send a special delimiter character which acts as a boundary between the messages. The receiving side should receive chunks until it receives the boundary character.

3. Describe the general differences between stateful and stateless protocols.

Stateless protocols don't need to maintain any per-connection state which significantly simplifies their design and performance.
Stateful protocols require the server to keep a state for every connection which can make the implementation more complex and require additional
system resources.

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

If you are building something that prioritizes speed and performance over reliability then you would chose UDP over TCP.
Things like video games and live streaming use UDP because they prefer fast response time over 100% reliability.

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

OS enables network communication using sockets. Sockets give you the ability to connect, listen, send, and receive data over the network
without having to interact with networking hardware.

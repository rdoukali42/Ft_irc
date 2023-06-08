# IRC (Internet Relay Chat) Server

This project is an IRC server implemented in C++98. It allows users to connect via IRC clients and communicate through real-time messaging, both in public channels and through private messages.

The server supports the following features:

<ul>
    <li>User Management: Users can set their nickname and username.</li>
    <li>Authentication: Clients need to provide a password to connect to the server.</li>
    <li>Channel Management: Users can join channels, and all messages sent to a channel are forwarded to every client in that channel.</li>
    <li>Private Messaging: Users can send and receive private messages.</li>
    <li>Operator Privileges: The server distinguishes between regular users and operators. Operators have additional permissions and can perform specific commands.</li>
    <li>Channel Commands: Operators can kick users from a channel, invite users to a channel, change or view the channel topic, and modify the channel mode (invite-only, topic restrictions, password, user limit).</li>
</ul>

## Requirements

<ul>
    <li>C++98 compliant compiler</li>
    <li>Unix-like operating system (Linux, macOS)</li>
</ul>

## Compilation and Usage

1. Clone the repository:

```
git clone https://github.com/Shetteemah/ft_irc.git
```

2. Change to the project directory:

```
cd ft_irc
```

3. Compile the server using the provided Makefile:

```
make
```

4. Run the server with the desired port number and password:

```
./ircserver <port> <password>
```


## IRC Client Setup

To test the server, you need an IRC client. The reference client provided during the evaluation process can be used. Configure the client to connect to the server using the specified port and password.

## Supported IRC Commands

The server supports the following IRC commands:

<ul>
    <li>JOIN: Join a channel.</li>
    <li>PRIVMSG: Send a message to a user or a channel.</li>
</ul>

Additionally, operators can use the following commands:

<ul>
    <li>KICK: Kick a user from a channel.</li>
    <li>INVITE: Invite a user to a channel.</li>
    <li>TOPIC: Change or view the channel topic.</li>
    <li>MODE: Change the channel mode.</li>
</ul>

## Additional Features

In addition to the mandatory requirements, the server includes the following bonus features:

<ul>
    <li>File Transfer: Handle file transfer between clients.</li>
    <li>Bot: Implement a bot with specific functionalities.</li>
</ul>

Please note that the bonus features will only be evaluated if the mandatory requirements are implemented perfectly.

## Testing

It is recommended to thoroughly test the server with different scenarios, including handling multiple clients, various commands, and error conditions. You can use the provided reference client or any other IRC client to test the server's functionality.

## Contribution

This project is a group project built by:

Anas Dinary ([adinari](https://github.com/adinari42))<br/>
Reda Doukali ([rdoukali](https://github.com/rdoukali42))</br>
Shettima Ali ([sheali](https://github.com/Shetteemah))</br>

## Language

Built with: </br>
[![My Skills](https://skillicons.dev/icons?i=cpp)](https://skillicons.dev)

## License

This project is licensed under the [MIT License](https://github.com/Shetteemah/ft_irc/blob/main/LICENSE).

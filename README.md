# IRC (Internet Relay Chat) Server

This project is an IRC server implemented in C++98. It allows users to connect via IRC clients (netcat and Lime Chat were used for this project) and communicate through real-time messaging, both in public channels and through private messages.

## The server supports the following features:

<ul>
    <li>User Management: Users can set their nickname and username.</li>
    <li>Authentication: Clients need to provide a password to connect to the server.</li>
    <li>Channel Management: Users can join channels, and all messages sent to a channel are forwarded to every client in that channel.</li>
    <li>Private Messaging: Users can send and receive private messages.</li>
    <li>Operator Privileges: The server distinguishes between regular users and operators (admins). Operators have additional permissions and can perform specific commands.</li>
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

To test the server, you need an IRC client. The server interacts greatly with the `netcat` client and the reference client for us was `Lime chat` IRC client. Configure the client to connect to the server using the specified port and password.

## Connecting with netcat (nc) Client

### To join the server with an nc client, follow these steps:

1. Give 3 parameters: `nc` `<host>` `<port>`.
2. Enter the password: `PASS` `<password>`.
3. Give the username: `USER` `<username>`.
4. Give the nickname: `NICK` `<nickname>`.

## Connecting with Lime Chat IRC Client

### To connect with the `Lime Chat` IRC client, enter the following details in the "New Server" window:

1. Enter a network name of your choice.
2. In the server field, enter the `<host>`.
3. Enter the `<port>` in the port field.
4. Enter the `<server password>` in the Server Password field.
5. Enter the client `<username>` in the Nickname field.
6. Click `Ok` to proceed.

## Supported IRC Commands

### The server supports the following IRC commands:

- `JOIN`: Usage: `JOIN` `<#channel name>` `<message (optional)>`. It is used to create a new channel if a channel with that name doesn't already exist. Otherwise, the command lets you join an existing channel. All channel names must start with a `#`.
- `PRIVMSG`:
  Usage:
  - `PRIVMSG` `<username>`Send a message to a user.
  - `PRIVMSG` `<#channel name>` `<username>`Send a message to all users in a channel.
- `LIST`: This command lets you see all the information of all the channels existing on the server.
- `WHOIS`: Usage: `WHOIS` `<username>`. This command pulls up an info sheet of the user.
- `INVITE`: Usage: `INVITE` `<#channel name>` `<invitee username>`. This command invites the specified user to the specified channel.
- `MODE`: The `MODE` command can be used by an admin to change the channel’s mode in a number of ways:
  - `MODE` `+/-i` `<#channel name>`: Set/remove Invite-only channel.
  - `MODE` `+/-t` `<#channel name>`: Set/remove the restrictions of the TOPIC command to channel operators.
  - `MODE` `+/-k` `<#channel name>`: Set/remove the channel key (password).
  - `MODE` `+/-o` `<#channel name>`: Give/take channel operator (admin) privilege.
- `KICK`: Usage: `KICK` `<username>`: Ejects the client whose username is given from the channel.
- `QUIT`: This command allows the user to disconnect from the server.
- `EXIT`: This command can be used by any user to close the server.

**Please note that Lime Chat recognizes command keywords in both upper and lower case, while the `nc` client only recognizes upper case commands.


## Testing

We welcome you to test the server with different scenarios, including handling multiple clients, various commands, and error conditions. Specifically with `lime chat` IRC client. Feel free to report bugs or make recommendations to any of the group members listed below.

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

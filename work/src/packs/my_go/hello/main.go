package main

import (
	"bytes"
	"fmt"
	"log"
	"net"

	"golang.org/x/crypto/ssh"
)

func main() {
	SSH("root", "123456789+0Aa", "120.79.207.54:22")
}

func SSH(user, password, ip_port string) {
	ce := func(err error, msg string) {
		if err != nil {
			log.Fatalf("%s error: %v", msg, err)
			return
		}
	}
	Client, err := ssh.Dial("tcp", ip_port, &ssh.ClientConfig{
		User: user,
		Auth: []ssh.AuthMethod{
			ssh.Password(password),
		},
		//需要验证服务端，不做验证返回nil就可以，点击HostKeyCallback看源码就知道了
		HostKeyCallback: func(hostname string, remote net.Addr, key ssh.PublicKey) error {
			return nil
		},
	})

	/*modes := ssh.TerminalModes{
		ssh.ECHO:          0,
		ssh.TTY_OP_ISPEED: 14400,
		ssh.TTY_OP_OSPEED: 14400,
	}*/

	session, err := Client.NewSession()
	defer session.Close()
	if err != nil {
		ce(err, "creat a new session")
	}

	var b bytes.Buffer
	session.Stdout = &b
	if err := session.Run("mysql -uroot -p111"); err != nil {
		panic("Failed to run: " + err.Error())
	}
	/*if err := session.Run("show databases;"); err != nil {
		panic("Failed to run: " + err.Error())
	}*/
	fmt.Println(b.String())
}

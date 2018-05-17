package main

import (
	"bufio"
	"bytes"
	"fmt"
	"net"
	"sync"
	"time"
)

const (
	Conpro    = "tcp"
	Localaddr = "172.18.151.39"
	Localhost = "7000"
	Delimiter = '\t'
)

var clientid int

func init() {
	clientid = 0
}

//并发处理客户端
func Solve(conn net.Conn, id int) {
	buffer := bufio.NewReader(conn)
	for {
		mes, err := Getmessage(conn, buffer)
		if err != nil {
			wg.Done()
			fmt.Printf("Recived message from client[%d] fail.(%s)\n", id, err)
			return
		}
		fmt.Printf("Recived message from client[%d] successfully(%s)\n", id, mes)
		/*var returnmes string
		fmt.Scanln(&returnmes)
		Sendmessage(conn, returnmes)*/
	}
}

func Getmessage(conn net.Conn, buffer *bufio.Reader) (string, error) {
	ReadBytes, err := buffer.ReadBytes(Delimiter)
	if err != nil {
		return "", err
	}
	return string(ReadBytes), nil
}

var wg sync.WaitGroup

func LocalServer() {
	defer wg.Done()
	listener, err := net.Listen(Conpro, fmt.Sprintf("%s:%s", Localaddr, Localhost))
	defer listener.Close()
	if err != nil {
		fmt.Printf("The server started fail.(%s)\n", err)
		return
	}
	fmt.Printf("The server started successfully.\n")
	for i := 0; i < 3; i++ {
		conn, err := listener.Accept()
		if err != nil {
			fmt.Printf("The client comes but connected fail!(%s)\n", err)
			break
		}
		fmt.Printf("The client[%d](%s) connected successfully.\n", clientid, conn.RemoteAddr())
		//var returnmes string
		//fmt.Scanln(&returnmes)
		Sendmessage(conn, "呵呵")
		fmt.Println("66")
		go Solve(conn, clientid)
		clientid++
	}
}

func Client(str string, id string) {
	//defer wg.Done()
	conn, err := net.DialTimeout(Conpro, fmt.Sprintf("%s:%s", Localaddr, Localhost), 2*time.Second)
	defer conn.Close()
	defer func() {
		fmt.Printf("client[%s]: Connection closed!\n", id)
	}()
	if err != nil {
		fmt.Printf("Client[%s]: Connected to the server fail(%s)\n", id, err)
		return
	}
	fmt.Printf("Client[%s]: Connected to the server successfully\n", id)
	len, err := Sendmessage(conn, str)
	if err != nil {
		fmt.Printf("Client[%s]: An error comes when send message to server(%s)\n", id, err)
	}
	fmt.Printf("Client[%s]: Send message(%s) with %d bytes successfully!\n", id, str, len)
}

func Sendmessage(conn net.Conn, str string) (int, error) {
	var buffer bytes.Buffer
	buffer.WriteString(str)
	buffer.WriteByte(Delimiter)
	return conn.Write(buffer.Bytes())
}

func main() {
	wg.Add(3)
	go LocalServer()
	time.Sleep(500 * time.Millisecond)
	go Client("我是SB成俊林", "陈俊林")
	go Client("我是SB做起林", "卓奇林")
	wg.Wait()
}

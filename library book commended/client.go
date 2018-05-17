package main

import (
	"bufio"
	"bytes"
	"fmt"
	"net"
	"os"
	"sync"
	"time"
)

const (
	Conpro    = "tcp"
	Localaddr = "172.18.151.39"
	Localhost = "7000"
	Delimiter = '\n'
)

var clientid int

func init() {
	clientid = 0
}
//并发处理客户端
func Solve(conn net.Conn, id int) {
	var brk bool = false
	go func() { //从客户端接收信息
		buffer := bufio.NewReader(conn)
		for {
			mes, err := Getmessage(conn, buffer)
			if err != nil {
				wg.Done()
				fmt.Printf("Recived message from client[%d] fail.(%s)\n", id, err)
				brk = true
				return
			}
			fmt.Printf("Recived message from client[%d] successfully:%s", id, mes)
		}
	}()
	go func() { //发送信息给客户端
		for {
			len, err := Sendmessage(conn)
			if err != nil {
				fmt.Printf("发送失败:%s\n", err)
			} else {
				fmt.Printf("发送成功%d个字节!\n", len)
			}
			if brk {
				return
			}
		}
	}()
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
	for {
		wg.Add(1)
		conn, err := listener.Accept()
		if err != nil {
			fmt.Printf("The client comes but connected fail!(%s)\n", err)
			break
		}
		fmt.Printf("The client[%d](%s) connected successfully.\n", clientid, conn.RemoteAddr())
		go Solve(conn, clientid)
		clientid++
	}
}

func Sendmessage(conn net.Conn) (int, error) {
	inputReader := bufio.NewReader(os.Stdin)
	input, _ := inputReader.ReadString('\n')
	var buffer bytes.Buffer
	buffer.WriteString(input)
	return conn.Write(buffer.Bytes())
}

func main() {
	wg.Add(1)
	go LocalServer()
	wg.Wait()
}

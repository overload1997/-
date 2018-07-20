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
	Closeflag = "#close#\n"
)

var wg sync.WaitGroup

//处理客户端
func Getmessage(conn net.Conn, buffer *bufio.Reader) (string, error) {
	ReadBytes, err := buffer.ReadBytes(Delimiter)
	if err != nil {
		return "", err
	}
	return string(ReadBytes), nil
}

func Sendmessage(conn net.Conn) (int, error) {
	inputReader := bufio.NewReader(os.Stdin)
	input, _ := inputReader.ReadString('\n')
	var buffer bytes.Buffer
	if input == Closeflag {
		wg.Done()
		conn.Close()
		return 0, nil
	}
	buffer.WriteString(input)
	//buffer.WriteByte(Delimiter)
	return conn.Write(buffer.Bytes())
}

func Client() {
	conn, err := net.DialTimeout(Conpro, fmt.Sprintf("%s:%s", Localaddr, Localhost), 2*time.Second)
	if err != nil {
		fmt.Printf("Connected to server fail!(%s %s)\n", conn, err)
		return
	}
	fmt.Printf("Connected to server successfully\n")
	time.Sleep(200 * time.Millisecond)
	go func() { //发信息
		for {
			len, err := Sendmessage(conn)
			if err != nil {
				fmt.Printf("Send message fail!(%s)\n", err)
				break
			}
			fmt.Printf("Send message with %d byte successfully!\n", len)
		}
	}()

	go func() { //读信息
		buffer := bufio.NewReader(conn)
		for {
			message, err := Getmessage(conn, buffer)
			if err != nil {
				fmt.Printf("Get message fail!(%s)\n", err)
			}
			fmt.Printf("Get message from server:%s", message)
		}
	}()
}

func main() {
	wg.Add(1)
	go Client()
	time.Sleep(1 * time.Second)
	wg.Wait()
}

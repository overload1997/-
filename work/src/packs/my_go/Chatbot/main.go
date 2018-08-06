package main

import (
	"fmt"
	"packs/Talk"
)

type People string

type Chatbot interface {
	Move(direction string)
	Ifempty() bool
	Talk.Talk
}

func (rb *People) Move(direction string) {
	if direction == "left" {
		fmt.Printf("the robot move to left\n")
	} else if direction == "up" {
		fmt.Printf("the robot move to up\n")
	} else if direction == "right" {
		fmt.Printf("the robot move to right\n")
	} else if direction == "down" {
		fmt.Printf("the robot move to down\n")
	} else {
		fmt.Printf("please input a corret direction(left|right|up|down)\n")
	}
}

func (rb *People) Ifempty() bool {
	return rb == nil
}

func main() {
	rb := new(People)
	rb.Move("left")
	if rb.Ifempty() {
		fmt.Printf("robot is exist\n")
	} else {
		fmt.Printf("robot is dead\n")
	}
	//不能调用其他包里的接口，所以这里会报错，应该把tk.go里的Talk接口相关内容全部复制过来
	rb.Output("hi! I can use you function!")
}

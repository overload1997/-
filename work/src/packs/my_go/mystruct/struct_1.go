package main

import (
	"fmt"
)

type Node struct {
	u, v, w int
}

func init() {
	defer fmt.Printf("Function defer\n") //延迟函数,是最慢执行的，在函数体中的位置不限
	fmt.Printf("Function init!\n")
}

func ErrorPrint() {
	for i := 0; i < 5; i++ {
		defer func() {
			fmt.Printf("%d ", i)
		}()
	}
	fmt.Printf("\n")
}

func CorrectPrint() {
	for i := 0; i < 5; i++ {
		defer func(n int) {
			fmt.Printf("%d ", n)
		}(i) //这个括号是顺便调用的意思？
	}
	fmt.Printf("\n")
}

func main() {
	//赋值方法1
	var node Node
	node = Node{u: 1, v: 2, w: 3}
	fmt.Println(node.u, node.v, node.w)

	//赋值方法2
	var node1 Node
	node1 = Node{1, 2, 3}
	fmt.Println(node1.u, node1.v, node1.w)

	str := "Englis"
	switch str {
	/*case "math":
		fmt.Printf("I like %s\n", str)
	case "Chinese":
		fmt.Printf("I like %s\n", str)*/
	case "English", "Math", "Chinese":
		fmt.Println("Here")
		fmt.Printf("I like %s and my girlfriend\n", str)
	default:
		fmt.Printf("I like others\n")
	}

	//类型switch 语句

	var v interface{}
	v = "overload"

	switch v.(type) {
	case int, string:
		fmt.Printf("This is a basic type\n")
	case interface{}:
		fmt.Printf("Good boy,you have learned the interface!\n")
	default:
		fmt.Printf("Sorry!I can't identify this type!")
	}

	for i := 0; i < 10; i++ {
		fmt.Printf("%d ", i)
	}
	fmt.Printf("\n")

	array := []int{1, 2, 4, 8, 16, 32, 64, 128, 256, 512}
	for index, val := range array { //range 迭代
		fmt.Printf("(2^%d=%d)", index, val) //第一个位置是索引  第二个位置是取当前索引的元素
	}
	fmt.Printf("\n")
	ErrorPrint()
	CorrectPrint()
}

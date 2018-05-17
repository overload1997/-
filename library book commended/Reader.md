喵喵瞄～
服务器端开发ing...
debug:
客户端

func f() {
  for {
    fmt.Printf("...")
  }
  wg.Done()
}


func main() {
  wg.Add(1)
  f()
  wg.Wait()
}

wg.wait() 是不能阻塞for的，这个程序会一执行就结束

package main

/*
	Simple C/S rate-limited, parallel, non-blocking RPC system.
*/
import (
	"fmt"
	"math/rand"
)

const (
	MaxOutStanding int = 10
)

type Request struct {
	args    []int
	fun     func([]int) int
	resChan chan int
}

func sum(a []int) (s int) {
	for _, v := range a {
		s += v
	}
	return
}

func Client(requests chan *Request) {
	args := make([]int, rand.Intn(10)+2)
	for i := range args {
		args[i] = rand.Intn(100) + 1
	}
	req := &Request{args, sum, make(chan int)}
	requests <- req
	fmt.Printf("Sum of %v is %d.\n", req.args, <-req.resChan)
}

func handle(queue chan *Request) {
	for req := range queue {
		//process(r)
		req.resChan <- req.fun(req.args)
	}
}

func Server(clients chan *Request, quit chan bool) {
	for i := 0; i < MaxOutStanding; i++ {
		go handle(clients)
	}
	<-quit
}
func RunClients(queue chan *Request, quit chan bool) {
	for {
		Client(queue)
		select {
		case <-quit:
			println("shutdown clients.")
		default:
			//println("next client...")
		}
	}
}
func main() {
	queue := make(chan *Request)
	quit := make(chan bool, 1)

	go Server(queue, quit)
	go RunClients(queue, quit)

	var i int
	fmt.Scan(&i)
	quit <- true
	quit <- true
	fmt.Println("Quit.")
}

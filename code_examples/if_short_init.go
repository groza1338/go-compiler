package main

import "fmt"

func main() {
    if v := 2 + 3; v == 5 {
        fmt.Print("ok")
    } else {
        fmt.Print("fail")
    }
}

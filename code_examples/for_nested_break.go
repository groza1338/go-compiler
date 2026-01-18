package main

import "fmt"

func main() {
    c := 0
    for j := 0; j < 2; j++ {
        for i := 0; i < 3; i++ {
            if i == 1 {
                break
            }
            c = c + 1
        }
    }
    fmt.Print(c)
}

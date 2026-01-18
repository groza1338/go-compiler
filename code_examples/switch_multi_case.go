package main

import "fmt"

func main() {
    x := 3
    switch x {
    case 1, 3, 5:
        fmt.Print("odd")
    case 2, 4:
        fmt.Print("even")
    }
}

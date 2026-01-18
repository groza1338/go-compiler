package main

import "fmt"

func greet(name string) string {
    return "hi " + name
}

func main() {
    fmt.Print(greet("go"))
}

package main

func pair() (int, int) {
    return 1, 2
}

func f() (int, int) {
    return pair()
}

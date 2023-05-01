int main() {
    bool a = true;
    bool b = true;
    int x = 7;
    int y = 4;
    int z = 72;
    if (a) {
        if (b) {
            return x+y;
        }
    } else {
        return y+z;
    }
    return x+y+z;
}
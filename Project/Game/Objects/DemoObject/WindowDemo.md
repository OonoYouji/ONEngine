```c++

class Window {
public:
    Window() {}
    ~Window() {}

    void Initialize();
    void Update();

private:

    bool isActive_;
    bool isHover_;

    HWND hwnd_;
};


```


```c++

int main() {


    while(!win->process()) {
        /// debug update
        debug->update();
        debug->draw();

        /// game update
        game->update();
        game->draw();
    }

    return 0;
}

```
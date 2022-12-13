
```
watcher () { 
    local f=$1;
    local ff=${f%.*};
    while inotifywait $f 2> /dev/null; do
        clear;
        g++ $f -o $ff $(pkg-config --cflags --libs gmock);
        if [ $? -eq 0 ]; then
            echo "----------- oOo ------------";
            ./$ff;
        fi;
    done
}
```

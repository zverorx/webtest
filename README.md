# webtest

HTTP test server for measuring download speed.

## Compilation

Go to the source directory, and run:

```
make
```

The executable file will be in the bin/ directory.

## Usage

### Start the server
```
./webtest <port>
```

### Measure download speed 

```
time curl http://host:port/<size><B|K|M|G> > /dev/null
```

**Limits: file size 0B–10G, max 10 parallel clients.**

## License

This project is licensed under the GPLv3. See the LICENSE file for more details.

## Feedback

To contact the developer, you can email zveror1806@gmail.com.

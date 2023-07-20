# Weather in terminal
 
Made to more easily find the current weather in the terminal.

## Get started

Requirements for compiling the package:

* Cmake v3.15 or higher
* Make
* GCC

### Building the application

Build the application with the following commands: 
```sh
cmake .
make
```

### Installation of the application

[weather-extension]: https://github.com/PerssonAlbin/tmux
It is recommended to create a symlink from the application to your bin folder (This is required to work with the [Catppuccin weather status bar][weather-extension]).

### Setup the config

After running the application once, the config will be created in `(home directory)/.config/.weather_config.json`. The config should look like this:

```json
{
    "api_key": "",
    "city": "",
    "temp_unit": ""
}
```

[open-weather]: https://openweathermap.org/api/

| Config keys | Explanations                                                                 |
| :---------- | :--------------------------------------------------------------------------- |
| api_key     | An API key can be retrieved for free from [OpenWeather's API][open-weather]. |
| city        | Any city you would like in English.                                          |
| temp_unit   | The following temperature units are accepted: Celsius, Fahrenheit & Kelvin.  |



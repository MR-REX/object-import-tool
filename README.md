# Object Import Tool

Object Import Tool is a tool for World Editor that allows you to place objects on the map using CSV files.

> [!IMPORTANT]
> At the same time, for the proper operation of the tool, it is expected that the specified CSV files will
> comply with the **RFC 4180** standard. For any other situations, manual configuration of file parsing is possible.

## Features

- Ability to change CSV file parsing parameters.
- Ability to adjust the position of imported objects.
- Ability to change the scale of imported objects.

## Usage

### Expected CSV file formats

1. Using **Quaternions**.
```
[ ResourcePath ] [ PosX ] [ PosY ] [ PosZ ] [ QuatX ] [ QuatY ] [ QuatZ ] [ QuatW ] [ Scale ]
```

2. Using **Euler (Tate-Bryan) angles**.
```
[ ResourcePath ] [ PosX ] [ PosY ] [ PosZ ] [ Yaw ] [ Pitch ] [ Roll ] [ Scale ]
```

## License

This project is licensed under the **Arma Public License Share Alike (APL-SA)**. See the
[LICENSE](https://github.com/MR-REX/object-import-tool/blob/main/LICENSE) file for details.

## Acknowledgments

A special thanks to **Bohemia Interactive** for creating Arma Reforger.

## Support and Feedback

If you encounter any issues or have suggestions for improvement, please
[open an issue](https://github.com/MR-REX/object-import-tool/issues) on GitHub.

## Links

- [Arma Reforger Workshop Page](https://reforger.armaplatform.com/workshop/6452AEF260F91B98-ObjectImportTool)
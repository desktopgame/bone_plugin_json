# bone_plugin_json

bone のプラグイン実装サンプルです。  
json とオブジェクトの相互変換の機能を提供します。

# sample

ffi_json_test.bn

```
{} <- load("ffi_json.bn");
{} <- load("file.bn");

json := json2obj("sample.json");
stdout.puts(json.name);
stdout.puts(json.resolutions[0].width.to_string());
stdout.puts(json.resolutions[0].height.to_string());

stdout.puts(json.to_string());

json.name := "HogeHoge";
file := open("sample2.json", "w");
defer file.close();
file.puts(obj2json(json));
```

sample.json

```
{
  "name": "Awesome 4K",
  "resolutions": [
    {
      "width": 1280,
      "height": 720
    },
    {
      "width": 1920,
      "height": 1080
    },
    {
      "width": 3840,
      "height": 2160
    }
  ]
}
```

sample2.json

```
{
	"name":	"HogeHoge",
	"resolutions":	[{
			"width":	1280,
			"height":	720
		}, {
			"width":	1920,
			"height":	1080
		}, {
			"width":	3840,
			"height":	2160
		}]
}

```

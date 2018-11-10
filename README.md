# libnbfx

## What is it

libnbfx is C++ library for MS-NBFX serialization and deserialization. 

MS-NBFX is Microsoft's .NET binary XML format that is primarily used in WCF. Specification is available [here](https://winprotocoldoc.blob.core.windows.net/productionwindowsarchives/MC-NBFX/[MC-NBFX].pdf).

Related C# code:

```c#
using(var reader=XmlDictionaryReader.CreateBinaryReader(stream, quotas))
{
    var object = serializer.Deserialize(reader);
}
```

## Building

```bash
mkdir build
cd ./build
cmake ..
make
make test
```

## Usage

### Serialization

```c++
using namespace nbfx;

auto root = 
    NbfxElement(L"task", { 
            NbfxAttribute(QName(L"i", L"xmlns"), 
                NbfxValue(L"http://www.w3.org/2001/XMLSchema-instance")) 
        }, {
            NbfxElement(L"Started", {}, NbfxValue(c->GetStartedTimestamp())),
            NbfxElement(L"Completed",{}, NbfxValue(c->GetLastEventTimestamp()))				
        });

if(condition)
{
    root.children().push_back(NbfxElement(L"SomeData", {}, NbfxValue(L"123")));
}

std::vector<uint8_t>    buffer;
serialize(root, std::back_inserter(buffer));
```

### Deserialization

```c++
const auto root = nbfx::parse(buffer.begin());
if(const auto someData = root.find_descendant(L"SomeData"))
{
    std::cout << someData->value().string();
}
```

## Contribution

Yes, please.
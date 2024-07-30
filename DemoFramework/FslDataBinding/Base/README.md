# FslDataBinding

A new experimental very basic data binding library.
It's only intended to provide the basic functionality needed by the demo-apps.

It currently supports

- One way binding.

Some of the original design goals.

- Have a closer to zero size cost when a property is not bound to anything (The same as declaring member of that type inside the class).

## Binding

### One-way bind

It's easy to bind properties of the same types to eachother. For example to use ```Property1``` on object ```t0``` as source for target ```Property0``` on object ```t1```.

```C++
ExampleA t0;    // Property1 is a int32_t
ExampleB t1;    // Property0 is a int32_t

// target t1.Property0 with a binding that has t0.Property1 as source (types match)
t1.SetBinding(ExampleB::Property0, t0.GetPropertyHandle(ExampleA::Property1));
```

Alternative the 'long' binding form could be used as well

```C++
ExampleA t0;    // Property1 is a int32_t
ExampleB t1;    // Property0 is a int32_t

// target t1.Property0 with a binding that has t0.Property1 as source (types match)
t1.SetBinding(ExampleB::Property0, Binding(t0.GetPropertyHandle(ExampleA::Property1)));
```

### Two-way bind

It's easy to bind properties of the same types to eachother. For example to use ```Property1``` on object ```t0``` as source for target ```Property0``` on object ```t1```.

```C++
ExampleA t0;    // Property1 is a int32_t
ExampleB t1;    // Property0 is a int32_t

// target t1.Property0 with a binding that has t0.Property1 as source (types match)
t1.SetBinding(ExampleB::Property0, t0.GetPropertyHandle(ExampleA::Property1), DataBinding::BindingMode::TwoWay);
```

Alternative the 'long' binding form could be used as well

```C++
ExampleA t0;    // Property1 is a int32_t
ExampleB t1;    // Property0 is a int32_t

// target t1.Property0 with a binding that has t0.Property1 as source (types match)
t1.SetBinding(ExampleB::Property0, Binding(t0.GetPropertyHandle(ExampleA::Property1), DataBinding::BindingMode::TwoWay));
```


### One-way conversion binding

When the properties are type incompatible we will need to use a ```ConverterBinding```. So if ```t0.Property2``` is a ```float``` and ```t1.Property0``` is a ```int32``` we will need to add ConverterBinding to the binding request like this:

```C++
ExampleA t0;    // Property2 is a float
ExampleB t1;    // Property0 is a int32_t

// The actual conversion code
auto converterBinding = std::make_shared<Fsl::DataBinding::ConverterBinding<int32_t, float>>(
    [](const float value) { return static_cast<int32_t>(std::round(value)); }
);

// target t1.Property0 with the converter binding that has t0.Property2 as source
t1.SetBinding(ExampleB::Property0, Binding(converterBinding, t0.GetPropertyHandle(ExampleA::Property2)));
```

### Two-way conversion binding

When the properties are type incompatible we will need to use a ```ConverterBinding```. So if ```t0.Property2``` is a ```float``` and ```t1.Property0``` is a ```int32``` we will need to add ConverterBinding to the binding request like this:

```C++
ExampleA t0;    // Property2 is a float
ExampleB t1;    // Property0 is a int32_t

// The actual conversion code
auto convertingBinding = std::make_shared<Fsl::DataBinding::TwoWayConverterBinding<uint32_t, float>>(
  [](const float value) { return static_cast<uint32_t>(std::round(value)); }, 
  [](const uint32_t value) { return static_cast<float>(value); });

// target t1.Property0 with the converter binding that has t0.Property2 as source
t1.SetBinding(ExampleB::Property0, Binding(converterBinding, t0.GetPropertyHandle(ExampleA::Property2)));
```

### One-way multi conversion binding

Sometimes you want to bind multiple values to one target. For example if you have four color channels R,G,B,A and need to bind them to a property that takes a color.

```C++
Slider colorChannelR;   // PropertyValue is a uint8_t
Slider colorChannelG;   // PropertyValue is a uint8_t
Slider colorChannelB;   // PropertyValue is a uint8_t
Slider colorChannelA;   // PropertyValue is a uint8_t
Image exampleImage;     // PropertyContentColor is a Color

// Acquire all the source handles that need to be converted via the multi-converter
const auto hSourceR = sliderR.GetPropertyHandle(Slider::PropertyValue);
const auto hSourceG = sliderG.GetPropertyHandle(Slider::PropertyValue);
const auto hSourceB = sliderB.GetPropertyHandle(Slider::PropertyValue);
const auto hSourceA = sliderA.GetPropertyHandle(Slider::PropertyValue);

// The actual conversion code
auto converterBinding = std::make_shared<Fsl::DataBinding::MultiConverterBinding<Color, uint8_t, uint8_t, uint8_t, uint8_t>>
(
    [](const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) 
    { 
        return Color(r,g,b,a); 
    }
);

// target the exampleImage.PropertyContentColor with the multi-converter binding and its sources.
exampleImage.SetBinding(Image::PropertyContentColor, Binding(converterBinding, hSourceR, hSourceG, hSourceB, hSourceA));
```

### Two-way multi conversion binding

Sometimes you want to bind multiple values to one target. For example if you have four color channels R,G,B,A and need to bind them to a property that takes a color.

```C++
Slider colorChannelR;   // PropertyValue is a uint8_t
Slider colorChannelG;   // PropertyValue is a uint8_t
Slider colorChannelB;   // PropertyValue is a uint8_t
Slider colorChannelA;   // PropertyValue is a uint8_t
Image exampleImage;     // PropertyContentColor is a Color

// Acquire all the source handles that need to be converted via the multi-converter
const auto hSourceR = sliderR.GetPropertyHandle(Slider::PropertyValue);
const auto hSourceG = sliderG.GetPropertyHandle(Slider::PropertyValue);
const auto hSourceB = sliderB.GetPropertyHandle(Slider::PropertyValue);
const auto hSourceA = sliderA.GetPropertyHandle(Slider::PropertyValue);

// The actual conversion code
 auto converterBinding = std::make_shared<Fsl::DataBinding::TwoWayMultiConverterBinding<Color, uint8_t, uint8_t, uint8_t, uint8_t>>(
   [](const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) { return Color(r, g, b, a); },
   [](const Color src) { return std::make_tuple(src.R(), src.G(), src.B(), src.A()); });

// target the exampleImage.PropertyContentColor with the multi-converter binding and its sources.
exampleImage.SetBinding(Image::PropertyContentColor, Binding(converterBinding, hSourceR, hSourceG, hSourceB, hSourceA));
```

## Known issues

* When setting a two-way binding programatically and the existing value is equal to the value being set then it will not update all bound values. This can cause problems if you have two sets to the same bind tree, the first sets the value of one property the second sets a value of another but the second set does not change the value of the property being set. Since the bind value propagation is done async it will mean that the first set in the bind tree will be the one being executed since the second one did not succeed in changing the current value.
* When writing two-way multi bind conversion functions its important to accurately handle the conversion so it makes sense both back and forth.

## Appendix: implementation research details

### Build size

Test                                                              | 1c, 11p | 5c, 55p | 10c, 110p | 15c, 165p | 20c, 220p | 25c, 275p | 30c, 330p
------------------------------------------------------------------|---------|---------|-----------|-----------|-----------|-----------|------------
Windows: get/set stored in separate definition. Optimized         | 225.792 | 255.488 |   294.912 |   327.680 |   390.144 |   390.144 |   422.912
Windows: get/set stored in separate definition. Initial           | 226.304 | 256.000 |   295.424 |   328.192 |   359.936 |   390.656 |   423.424
Windows: get/set stored in TypedDependencyProperty. Optimized     | 215.552 | 266.752 |   330.240 |   389.120 |   446.464 |   503.296 |   559.104
Windows: get/set stored in TypedDependencyProperty. Initial       | 217.088 | 283.648 |   366.080 |   432.128 |   495.104 |   556.544 |   617.984
Windows: get/set stored in TypedDependencyProperty. std::function | 237.568 | 373.248 |   544.768 |   705.024 |   863.232 | 1.021.440 | 1.179.648
Linux: get/set stored in separate definition. Optimized           | 227.656 | 276.808 |   342.344 |   416.072 |   481.608 |   551.240 |   620.872
Linux: get/set stored in TypedDependencyProperty. Optimized       | 198.984 | 264.520 |   342.344 |   428.360 |   510.280 |   596.296 |   678.216
Linux: get/set stored in TypedDependencyProperty. std::function   | 223.560 | 391.496 |   600.392 |   813.384 | 1.014.088 | 1.214.792 | 1.419.592

#### Notes

- Windows is using VS2022 17.1.1
- Linux is using gcc (Ubuntu 11.1.0-1ubuntu1~21.04) 11.1.0

#### Info

- c = classes
- p = properties
- size is in bytes

#### Runtime memory size

Test                                                              | Unbound property size    | Additional binding data | DependencyPropertyDefinition
------------------------------------------------------------------|--------------------------|-------------------------|--
Windows: get/set stored in separate definition                    | 4 + sizeof(type) + align | ?                       | X + sizeof(method call magic)
Windows: get/set stored in optimized TypedDependencyProperty      | 4 + sizeof(type) + align | ?                       | X
Windows: get/set stored in initial TypedDependencyProperty        | 4 + sizeof(type) + align | ?                       | X
Windows: get/set stored in TypedDependencyProperty. std::function | >136                     | ?                       | X

- method call magic = 32bytes on windows


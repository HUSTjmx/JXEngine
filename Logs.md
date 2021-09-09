# 2021年

## 9月

### 9.8~9.9

这两天主要是复习Learn Opengl的Uniform buffer这节，为此设计了一个新的核心类，`uniform Buffer`，主要是负责uniform缓冲的管理，挂载在Scene上，每个Scene有个此类的数组，暂时的逻辑关系就是如此简单。

唯一的问题，导致花了很长时间的是一直渲染错误，直到9号的中午采用蛮力法发现问题：我编写的新类没有问题，但是问题在于，当使用`Matrices`这个缓冲区时，我会关掉每个Mesh（具体调用Material的接口）的`SetVP`，而由于之前没有管理好代码，我的`Active()`函数的调用很混乱，`SetM`没有，而`setVP`有，但一旦我不调用`SetVP`，只使用`setM`，会导致当前处理的`shader id`根本没有设置，随意一致导致渲染失败。

所以，得到的教训是，对于一个常用的简单函数，最好使用环境单一，要么不开放给用户，要么开放（且最好提醒调用），这里我的建议是不开放`active()`。

烦死了，今天的引擎搭建暂时告一段落。

```c++
std::shared_ptr<UniformBuffer> matrixBuffer = std::make_shared<UniformBuffer>(CONFIG::SHADER_DEFAULT_UNIFORM_NAME::UNIFORM_BLOCK_NAME::MATRIX_COORD_SYSTEM);

UniformData ud1(CONFIG::SHADER_DEFAULT_UNIFORM_NAME::PROJECTION_MATRIX, UniformDataType::Mat4);
matrixBuffer->AddData(ud1);

UniformData ud2(CONFIG::SHADER_DEFAULT_UNIFORM_NAME::VIEW_MATRIX, UniformDataType::Mat4);
matrixBuffer->AddData(ud2);

matrixBuffer->Create();
matrixBuffer->LinkBindingPoint();

auto proj = scene.MainCamera().GetProjectionMatrix();

matrixBuffer->StoreData<glm::mat4>(proj, CONFIG::SHADER_DEFAULT_UNIFORM_NAME::PROJECTION_MATRIX);
MaterialPtr m = std::make_shared<Material>(sh);
```
SamplerState SamplerPointWrap : register(s0)
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
    AddressW = Wrap;
};
SamplerState SamplerPointClamp : register(s1)
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
    AddressW = Clamp;
};
SamplerState SamplerLinearWrap : register(s2)
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
    AddressW = Wrap;
};
SamplerState SamplerLinearClamp : register(s3)
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
    AddressW = Clamp;
};
SamplerState SamplerAnisotropicWrap : register(s4)
{
    Filter = ANISOTROPIC;
    AddressU = Wrap;
    AddressV = Wrap;
    AddressW = Wrap;
};
/*SamplerState SamplerAnisotropicClamp : register(s5)
{
    Filter = ANISOTROPIC;
    AddressU = Clamp;
    AddressV = Clamp;
    AddressW = Clamp;
};*/
SamplerState SamplerAnisotropicClamp : register(s5);
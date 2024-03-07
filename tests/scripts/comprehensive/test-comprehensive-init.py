from orbital import init, RunParam


e_param = RunParam("192.168.1.255", display="Gateway")
f_param = RunParam(str, display="Server")
g_param = RunParam(4, display="Rings")
h_param = RunParam(int, display="Level")
i_param = RunParam(5.0, display="Threshold (V)")
j_param = RunParam(float, display="Noise (dB)")
k_param = RunParam("192.168.1.255")
l_param = RunParam(str)
m_param = RunParam(4)
n_param = RunParam(int)
o_param = RunParam(5.0)
p_param = RunParam(float)

init(
    [
        (0, 1, 0, 2),
        (0, 1, 2, 1),
        (0, 3, 3, 1),
        (0, 1, 4, 2),
        (1, 1, 0, 1),
        (1, 1, 1, 2),
        (1, 4, 4, 2),
        (2, 2, 0, 3),
        (3, 1, 3, 1),
        (4, 1, 0, 4),
    ],
    a="",
    b="127.0.0.1",
    c=-100,
    d=1.602e-19,
    e=e_param,
    f=f_param,
    g=g_param,
    h=h_param,
    i=i_param,
    j=j_param,
    k=k_param,
    l=l_param,
    m=m_param,
    n=n_param,
    o=o_param,
    p=p_param,
)

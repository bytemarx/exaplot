# API


<code><em>class</em> exaplot.<b>RunParam(</b><em>default_or_type, /, display=None</em><b>)</b></code>

<dd>
<p>A specialized class for defining a run parameter. This object is used within the <code>init</code> function to define a run parameter with additional characteristics, such as how it is displayed.</p>
<p><em>default_or_type</em> can be either a default value or the value type. If a default value is given, the type will be inferred (equivalent to if a <code>RunParam</code> weren't used at all). If a value type is given, the GUI will not initialize the corresponding run argument.</p>
<p>The <em>display</em> argument is used as the string to display this parameter as in the GUI.</p>
</dd>

---

<code>exaplot.<b>init(</b><em>plots, /, **params</em><b>)</b></code>

<dd>
<p>Initializes the plots and run parameters.</p>
<p><em>plots</em> can be either a specific plot arrangement or simply the number of plots (using the default arrangement). If a specific plot arrangement is desired, it is recommended to use the plot editor to generate the plot arrangement. The default is a single plot.</p>
<p>The <em>params</em> keyword arguments specifies the run parameters. Each values may be either a value of type <code>str</code>, <code>int</code>, <code>float</code>, or <code>RunParam</code>.</p>
</dd>

---

<code>exaplot.<b>datafile(</b><em>*, enable=True, path=Path("data.hdf5"), prompt=False</em><b>)</b></code>

<dd>
<p>Configures the data file settings. Scripts that omit this function entirely will have their data file disabled by default.</p>
<p><em>enable</em> enables or disables writing to the data file.</p>
<p><em>path</em> specifies the path of the data file. This may be a "hard-coded" path or it may be generated dynamically at runtime via a callback function. The callback function must take no arguments and return a <code>PathLike</code> value. Note that if the same file is used for subsequent runs, the data file will be written over.</p>
<p>If the <em>prompt</em> flag is set, the GUI will prompt the user with the file path before each run to allow any changes (or simply as a means to explicitly confirm the file path).</p>
</dd>

---

<code>exaplot.<b>stop()</b></code>

<dd>
<p>Used to check if a stop signal has been received. If the <b>stop</b> button is clicked during a run, this will return <code>True</code>, otherwise this will return <code>False</code>.</p>
</dd>

---

<code>exaplot.<b>breakpoint()</b></code>

<dd>
<p>This is an alternative method for checking if the stop signal has been received. This function will raise a special <code>BaseException</code>-derived exception if a stop has been issued (with the intention that the exception will terminate the run). This is typically used as a convenience where the <code>stop</code> function would be insufficient (e.g. in nested loops).</p>
</dd>

---

<code>exaplot.<b>msg(</b><em>message, /, append=False</em><b>)</b></code>

<dd>
<p>Sets the message in the script message box.</p>
<p>If the <em>append</em> flag is set, messages will be appended to the existing message box contents.</p>
</dd>

---

<code>exaplot.<b>plot[</b><em>plot_id</em><b>](</b><em>...</em><b>)</b></code>

<dd>
<p>The <code>plot</code> variable acts as a container for the active plots (or more specifically, a container of the plot handles). A specific plot is accessed by using its plot ID as the index (the ID of each plot can be found within the Plot Editor).</p>
<p>Data is plotted (and stored to the data file, if enabled) by calling the plot handle directly:</p>
</dd>

```python
exaplot.plot[1](0, 1)
```

<dd>
<p>We can also assign a variable to a plot's handle:</p>
</dd>

```python
plot_1 = exaplot.plot[1]
plot_1(0, 1)
```

<dd>
<p>The plot handle's signature/overload depends on the plot's active plot type. Each overload has a <em>write</em> keyword argument that can be used to omit the data from being stored in the data file (if the data file is enabled).</p>

<p><h3>2-D plot:</h3></p>
<p><code><b>plot[</b><em>...</em><b>](</b><em>x, y, *, write=True</em><b>)</b></code></p>
<p>Both <em>x</em> and <em>y</em> can be either single values or a sequence of values (sequences must be the same length).</p>

<p><h3>Color map:</h3></p>
<p><code><b>plot[</b><em>...</em><b>](</b><em>col, row, value, *, write=True</em><b>)</b></code><br>
<code><b>plot[</b><em>...</em><b>](</b><em>row, values, *, write=True</em><b>)</b></code><br>
<code><b>plot[</b><em>...</em><b>](</b><em>frame, *, write=True</em><b>)</b></code></p>
<p>The colormap overloads allow a user to plot a cell at a time, a row of cells, or the entire frame (all cells).</p>
<p>The <em>col</em> and <em>row</em> arguments are <code>int</code> types and increase from left to right and down to up, respectively (in other words, the grid represents the first quadrant of a two-dimensional Cartesian coordinate system).</p>
<p>The <em>value</em> argument can be any value of type <code>Real</code> (e.g. integers or floating point numbers). Similarly, <em>values</em> is a sequence of <code>Real</code>-type values, and <em>frame</em> is a sequence of sequences of <code>Real</code>-type values.</p>
</dd>

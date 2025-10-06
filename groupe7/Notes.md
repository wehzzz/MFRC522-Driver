for regmap usage:

https://docs.huihoo.com/doxygen/linux/kernel/3.7/regmap_8c.html

Questions to answer:

- What do we want to do with the write function as it only takes something the user want to write, how do we know which register do they want to write to ?
  Answer: read the dawn subject of the project Anton !

- Why do we need to add the following macro in the source files as in gistre_card

```
MODULE_AUTHOR("Anton VELLA <anton.vella@epita.fr>");
MODULE_AUTHOR("Martin LEVESQUE <martin.levesque@epita.fr>");
MODULE_DESCRIPTION("MFRC522 card reader driver");
```

- The commands may take arguments and so will the associated functions so we can't have a function type for those

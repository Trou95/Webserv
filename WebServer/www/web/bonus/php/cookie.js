
function onPageLoad()
{
    const themeId = document.cookie?.split(";")?.find((theme) => theme.includes("theme"))?.split("=")[1];
    setTheme(themeId);
}

function setTheme(id)
{
    document.body.style.backgroundColor = id == 1 ? "#FFF" : "#000";
    document.cookie = "theme=" + id;
}

window.onload = start;

function start() {
  let brand = document.querySelector(".header_left");
  brand.addEventListener("click", () => {
    window.location.href = "/";
  });
}

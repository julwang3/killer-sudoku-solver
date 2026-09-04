/* Scripted using Claude Sonnet 5 */

(function () {
  const N = 9;
  const CAGE_COLORS = ["cage-1", "cage-2", "cage-3", "cage-4", "cage-5", "cage-6"];

  const state = {
    mode: "sudoku",               // "sudoku" | "killer"
    grid: makeGrid("."),
    given: makeGrid(false),
    solved: makeGrid(false),
    cages: [],                    // [{ sum, cells: [[r,c],...] }]
    cageTool: false,
    pendingCells: [],              // cells being selected for the cage in progress
  };
  let activeCell = { r: 0, c: 0 }; // last-focused cell, used by the number pad

  function makeGrid(fill) {
    return Array.from({ length: N }, () => Array.from({ length: N }, () => fill));
  }

  // ---------- DOM refs ----------
  const boardEl = document.getElementById("board");
  const statusEl = document.getElementById("status");
  const segControl = document.getElementById("seg-control");
  const labelSudoku = document.getElementById("label-sudoku");
  const labelKiller = document.getElementById("label-killer");
  const killerPanel = document.getElementById("killer-panel");
  const cageToolBtn = document.getElementById("cage-tool-btn");
  const cageBuilder = document.getElementById("cage-builder");
  const builderCount = document.getElementById("builder-count");
  const cageSumInput = document.getElementById("cage-sum-input");
  const confirmCageBtn = document.getElementById("confirm-cage-btn");
  const cancelCageBtn = document.getElementById("cancel-cage-btn");
  const cageListEl = document.getElementById("cage-list");
  const solveBtn = document.getElementById("solve-btn");
  const clearBtn = document.getElementById("clear-btn");
  const numpadEl = document.getElementById("numpad");
  const eraseBtn = document.getElementById("erase-btn");

  numpadEl.addEventListener("mousedown", (e) => {
    e.preventDefault(); // keep focus on the active cell, no matter what part of the numpad was clicked
  });
  numpadEl.addEventListener("click", (e) => {
    const btn = e.target.closest(".numpad-btn");
    if (!btn || btn.classList.contains("is-disabled")) return;
    const { r, c } = activeCell;
    setCell(r, c, btn.dataset.digit);
  });

  function updateNumpadEnabled() {
    const cageToolActive = state.mode === "killer" && state.cageTool;
    numpadEl.querySelectorAll("button").forEach((b) => { b.classList.toggle("is-disabled", cageToolActive); });
    if (!cageToolActive) {
      eraseBtn.classList.toggle("is-disabled", state.grid[activeCell.r][activeCell.c] === ".");
    }
  }

  // ---------- build the grid DOM once ----------
  const cellEls = [];
  for (let r = 0; r < N; r++) {
    const row = [];
    for (let c = 0; c < N; c++) {
      const cell = document.createElement("div");
      cell.className = "cell";
      if ((c + 1) % 3 === 0) cell.classList.add(c === N - 1 ? "col-edge" : "col-3n");
      if ((r + 1) % 3 === 0) cell.classList.add(r === N - 1 ? "row-edge" : "row-3n");

      const cageFill = document.createElement("div");
      cageFill.className = "cage-fill";
      const cageBorder = document.createElement("div");
      cageBorder.className = "cage-border";
      const cageSum = document.createElement("div");
      cageSum.className = "cage-sum";

      const input = document.createElement("input");
      input.setAttribute("inputmode", "numeric");
      input.setAttribute("maxlength", "1");
      input.dataset.r = r;
      input.dataset.c = c;

      input.addEventListener("mousedown", (e) => {
        if (state.mode === "killer" && state.cageTool) {
          e.preventDefault(); // block focus + native text selection during cage building
        }
      });
      input.addEventListener("click", () => onCellClick(r, c));
      input.addEventListener("keydown", (e) => onCellKeydown(e, r, c));
      input.addEventListener("focus", () => { input.select(); activeCell = { r, c }; updateNumpadEnabled(); });

      cell.appendChild(cageFill);
      cell.appendChild(cageBorder);
      cell.appendChild(cageSum);
      cell.appendChild(input);
      boardEl.appendChild(cell);
      row.push({ cell, input, cageFill, cageBorder, cageSum });
    }
    cellEls.push(row);
  }

  // ---------- cell interaction ----------
  function onCellClick(r, c) {
    if (state.mode === "killer" && state.cageTool) {
      toggleCellInBuilder(r, c);
    }
  }

  function onCellKeydown(e, r, c) {
    if (state.mode === "killer" && state.cageTool) {
      e.preventDefault();
      return;
    }
    if (/^[1-9]$/.test(e.key)) {
      setCell(r, c, e.key);
      e.preventDefault();
    } else if (e.key === "Backspace" || e.key === "Delete" || e.key === "0") {
      setCell(r, c, ".");
      e.preventDefault();
    } else if (e.key === "ArrowRight") { moveFocus(r, [r, Math.min(c + 1, N - 1)]); e.preventDefault(); }
    else if (e.key === "ArrowLeft")  { moveFocus(r, [r, Math.max(c - 1, 0)]); e.preventDefault(); }
    else if (e.key === "ArrowDown")  { moveFocus(r, [Math.min(r + 1, N - 1), c]); e.preventDefault(); }
    else if (e.key === "ArrowUp")    { moveFocus(r, [Math.max(r - 1, 0), c]); e.preventDefault(); }
  }

  function moveFocus(_r, [tr, tc]) {
    cellEls[tr][tc].input.focus();
  }

  function setCell(r, c, val) {
    state.grid[r][c] = val;
    state.given[r][c] = val !== ".";
    state.solved[r][c] = false;
    clearStatus();
    renderCell(r, c);
    updateNumpadEnabled();
  }

  // ---------- killer cage building ----------
  function toggleCellInBuilder(r, c) {
    if (isCellInCommittedCage(r, c)) return; // already claimed
    const idx = state.pendingCells.findIndex(([pr, pc]) => pr === r && pc === c);
    if (idx >= 0) state.pendingCells.splice(idx, 1);
    else state.pendingCells.push([r, c]);
    renderAll();
    updateBuilderUI();
  }

  function isCellInCommittedCage(r, c) {
    return state.cages.some((cage) => cage.cells.some(([cr, cc]) => cr === r && cc === c));
  }

  function updateBuilderUI() {
    const n = state.pendingCells.length;
    if (!state.cageTool) {
      builderCount.textContent = "Turn on the cage tool to select cells.";
    } else {
      builderCount.textContent = n === 0 ? "0 cells selected" : `${n} cell${n === 1 ? "" : "s"} selected`;
    }
    cageBuilder.classList.toggle("tool-off", !state.cageTool);
    cageSumInput.disabled = !state.cageTool;
    confirmCageBtn.disabled = !state.cageTool;
    cancelCageBtn.disabled = !state.cageTool;
  }

  cageToolBtn.addEventListener("click", () => {
    state.cageTool = !state.cageTool;
    cageToolBtn.textContent = "Cage Tool: " + (state.cageTool ? "On" : "Off");
    cageToolBtn.classList.toggle("active", state.cageTool);
    if (state.cageTool && document.activeElement && document.activeElement.tagName === "INPUT") {
      document.activeElement.blur();
    }
    if (!state.cageTool) { state.pendingCells = []; renderAll(); }
    updateBuilderUI();
    updateNumpadEnabled();
  });

  confirmCageBtn.addEventListener("click", () => {
    const sum = parseInt(cageSumInput.value, 10);
    if (state.pendingCells.length === 0) return;
    if (!Number.isFinite(sum) || sum < 1) {
      setStatus("Enter a valid sum for the cage.", "err");
      return;
    }
    state.cages.push({ sum, cells: state.pendingCells.map(([r, c]) => [r, c]) });
    state.pendingCells = [];
    cageSumInput.value = "";
    renderAll();
    renderCageList();
    updateBuilderUI();
    clearStatus();
  });

  cancelCageBtn.addEventListener("click", () => {
    state.pendingCells = [];
    renderAll();
    updateBuilderUI();
  });

  function removeCage(index) {
    state.cages.splice(index, 1);
    renderAll();
    renderCageList();
  }

  function renderCageList() {
    cageListEl.innerHTML = "";
    state.cages.forEach((cage, i) => {
      const li = document.createElement("li");
      const swatch = document.createElement("span");
      swatch.className = "swatch";
      swatch.style.background = `var(--${CAGE_COLORS[i % CAGE_COLORS.length]})`;
      const label = document.createElement("span");
      label.textContent = `Sum ${cage.sum} · ${cage.cells.length} cell${cage.cells.length === 1 ? "" : "s"}`;
      const del = document.createElement("button");
      del.textContent = "remove";
      del.addEventListener("click", () => removeCage(i));
      const left = document.createElement("span");
      left.appendChild(swatch);
      left.appendChild(label);
      li.appendChild(left);
      li.appendChild(del);
      cageListEl.appendChild(li);
    });
  }

  // ---------- rendering ----------
  function renderCell(r, c) {
    const { input, cell } = cellEls[r][c];
    input.value = state.grid[r][c] === "." ? "" : state.grid[r][c];
    cell.classList.toggle("given", state.given[r][c] && !state.solved[r][c]);
    cell.classList.toggle("solved", state.solved[r][c]);
  }

  function renderCages() {
    // clear
    for (let r = 0; r < N; r++) {
      for (let c = 0; c < N; c++) {
        const { cageFill, cageBorder, cageSum, cell } = cellEls[r][c];
        cageFill.style.background = "";
        cageBorder.style.borderWidth = "0";
        cageSum.textContent = "";
        cell.classList.toggle(
          "cage-pending",
          state.pendingCells.some(([pr, pc]) => pr === r && pc === c)
        );
      }
    }
    if (state.mode !== "killer") return;

    state.cages.forEach((cage, i) => {
      const colorVar = `var(--${CAGE_COLORS[i % CAGE_COLORS.length]})`;
      const cellSet = new Set(cage.cells.map(([r, c]) => `${r},${c}`));
      let topLeft = cage.cells[0];

      cage.cells.forEach(([r, c]) => {
        if (r < topLeft[0] || (r === topLeft[0] && c < topLeft[1])) topLeft = [r, c];
        const { cageFill, cageBorder } = cellEls[r][c];
        cageFill.style.background = colorVar;

        const top = !cellSet.has(`${r - 1},${c}`);
        const bottom = !cellSet.has(`${r + 1},${c}`);
        const left = !cellSet.has(`${r},${c - 1}`);
        const right = !cellSet.has(`${r},${c + 1}`);

        cageBorder.style.borderTopWidth = top ? "1.5px" : "0";
        cageBorder.style.borderBottomWidth = bottom ? "1.5px" : "0";
        cageBorder.style.borderLeftWidth = left ? "1.5px" : "0";
        cageBorder.style.borderRightWidth = right ? "1.5px" : "0";
      });

      cellEls[topLeft[0]][topLeft[1]].cageSum.textContent = cage.sum;
    });
  }

  function renderAll() {
    for (let r = 0; r < N; r++) for (let c = 0; c < N; c++) renderCell(r, c);
    renderCages();
  }

  function setStatus(msg, kind) {
    statusEl.textContent = msg;
    statusEl.className = "status" + (kind ? " " + kind : "");
  }
  function clearStatus() { setStatus("", ""); }

  // ---------- mode toggle ----------
  function setMode(mode) {
    state.mode = mode;
    state.cageTool = false;
    state.pendingCells = [];
    cageToolBtn.textContent = "Cage Tool: Off";
    cageToolBtn.classList.remove("active");
    updateBuilderUI();
    updateNumpadEnabled();

    const isKiller = mode === "killer";
    segControl.dataset.mode = mode;
    labelSudoku.setAttribute("aria-selected", String(!isKiller));
    labelKiller.setAttribute("aria-selected", String(isKiller));
    killerPanel.style.display = isKiller ? "block" : "none";
    clearStatus();
    renderAll();
  }

  labelSudoku.addEventListener("click", () => setMode("sudoku"));
  labelKiller.addEventListener("click", () => setMode("killer"));

  // ---------- clear ----------
  clearBtn.addEventListener("click", () => {
    state.grid = makeGrid(".");
    state.given = makeGrid(false);
    state.solved = makeGrid(false);
    state.cages = [];
    state.pendingCells = [];
    renderAll();
    renderCageList();
    clearStatus();
    updateNumpadEnabled();
  });

  // ---------- solve ----------
  let wasmModule = null;
  createModule()
    .then((Module) => {
      wasmModule = Module;
      solveBtn.disabled = false;
      setStatus("Ready.", "");
    })
    .catch((e) => setStatus("Failed to load solver: " + e, "err"));

  solveBtn.disabled = true;

  solveBtn.addEventListener("click", () => {
    if (!wasmModule) return;
    clearStatus();

    const board = state.grid.map((row) => row.slice());

    try {
      let resultJson;
      if (state.mode === "sudoku") {
        resultJson = wasmModule.solveSudoku(JSON.stringify({ board }));
      } else {
        const cages = state.cages.map((cage) => ({ sum: cage.sum, cells: cage.cells }));
        resultJson = wasmModule.solveKillerSudoku(JSON.stringify({ board, cages }));
      }

      const result = JSON.parse(resultJson);
      if (result.solved) {
        for (let r = 0; r < N; r++) {
          for (let c = 0; c < N; c++) {
            const wasGiven = state.given[r][c];
            state.grid[r][c] = result.board[r][c];
            state.solved[r][c] = !wasGiven;
          }
        }
        renderAll();
        setStatus("Solved.", "ok");
        updateNumpadEnabled();
      } else {
        setStatus(result.error || "This puzzle has no solution.", "err");
      }
    } catch (e) {
      setStatus("Error: " + e, "err");
    }
  });

  // ---------- init ----------
  renderAll();
  renderCageList();
  updateNumpadEnabled();
})();

const vscode = require("vscode");

function activate(context) {
  const disposable = vscode.commands.registerCommand(
    "sentenz.actionsTest",
    () => undefined,
  );

  context.subscriptions.push(disposable);
}

function deactivate() {}

module.exports = {
  activate,
  deactivate,
};
